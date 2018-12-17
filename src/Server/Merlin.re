open Rench;

module LspProtocol = Protocol;
module Log = Protocol.Log;

type mode =
  | Server
  | Single;

type t = {
  mode,
  merlinPath: string,
  additionalPaths: list(string),
};

[@deriving yojson({strict: false})]
type oneBasedLine = int;

[@deriving yojson({strict: false})]
type oneBasedCharacter = int;

module Position = {
  [@deriving yojson({strict: false})]
  type t = {
    line: oneBasedLine,
    col: oneBasedCharacter,
  };
};

module Protocol = {
  [@deriving yojson({strict: false})]
  type typeEnclosingResultItem = {
    tail: string,
    [@key "type"]
    enclosingType: string,
    [@key "start"]
    startPosition: Position.t,
    [@key "end"]
    endPosition: Position.t,
  };

  [@deriving yojson({strict: false})]
  type typeEnclosingResult = list(typeEnclosingResultItem);

  [@deriving yojson({strict: false})]
  type errorResultItem = {
    message: string,
    [@key "type"]
    errorType: string,
    [@key "start"]
    startPosition: Position.t,
    [@key "end"]
    endPosition: Position.t,
  };

  [@deriving yojson({strict: false})]
  type errorResult = list(errorResultItem);

  [@deriving yojson({strict: false})]
  type completionResultItem = {
    name: string,
    kind: string,
    desc: string,
  };

  [@deriving yojson({strict: false})]
  type completionResult = {entries: list(completionResultItem)};

  type response =
    | Return(Yojson.Safe.json)
    | Error(string);
};

let init = (mode: mode, merlinPath: string, additionalPaths: list(string)) => {
  let ret: t = {mode, merlinPath, additionalPaths};
  ret;
};

let getValueAsString = (json: Yojson.Safe.json) => {
  json |> Yojson.Safe.Util.member("value") |> Yojson.Safe.Util.to_string;
};

let _parse = (json: Yojson.Safe.json) => {
  let responseClass =
    json |> Yojson.Safe.Util.member("class") |> Yojson.Safe.Util.to_string;

  let isFailure =
    String.equal(responseClass, "error")
    || String.equal(responseClass, "failure")
    || String.equal(responseClass, "exception");
  let ret: Protocol.response =
    isFailure ?
      getValueAsString(json) |> Error :
      json |> Yojson.Safe.Util.member("value") |> Return;
  ret;
};

let _run = (~input: string, merlin: t, command: array(string)) => {
  let additionalPaths =
    String.concat(Rench.Path.pathSeparator, merlin.additionalPaths);
  let env = Environment.getEnvironmentVariables();
  let currentPath = EnvironmentVariables.getValue(env, "PATH");
  let augmentedPath =
    switch (currentPath) {
    | Some(v) => additionalPaths ++ ";" ++ v
    | None => additionalPaths
    };

  let updatedEnv = EnvironmentVariables.setValue(env, "PATH", augmentedPath);

  let opts = ChildProcess.SpawnSyncOptions.create(~input, ());

  let singleOrServer =
    switch (merlin.mode) {
    | Single => "single"
    | Server => "server"
    };

  let proc =
    ChildProcess.spawnSync(
      ~env=updatedEnv,
      ~opts,
      merlin.merlinPath,
      Array.append([|singleOrServer|], command),
    );

  proc.stdout |> Yojson.Safe.from_string;
};

let getTypeEnclosing =
    (merlin: t, position: Position.t, fileName: string, fileContents: string) => {
  let line = string_of_int(position.line);
  let col = string_of_int(position.col);
  let output =
    _run(
      ~input=fileContents,
      merlin,
      [|
        "type-enclosing",
        "-position",
        line ++ ":" ++ col,
        "-filename",
        fileName,
      |],
    );

  let ret = _parse(output);
  switch (ret) {
  | Error(v) => Error(v)
  | Return(json) =>
    json
    |> Protocol.typeEnclosingResult_of_yojson
    |> LspProtocol.Utility.getResultOrThrow
    |> Ok
  };
};

let getErrors = (merlin: t, fileName: string, fileContents: string) => {
  let output =
    _run(~input=fileContents, merlin, [|"errors", "-filename", fileName|]);

  let ret = _parse(output);
  switch (ret) {
  | Error(v) => Error(v)
  | Return(json) =>
    json
    |> Protocol.errorResult_of_yojson
    |> LspProtocol.Utility.getResultOrThrow
    |> Ok
  };
};

let getCompletePrefix =
    (
      merlin: t,
      prefix: string,
      position: Position.t,
      fileName: string,
      fileContents: string,
    ) => {
  let line = string_of_int(position.line);
  let col = string_of_int(position.col);
  let output =
    _run(
      ~input=fileContents,
      merlin,
      [|
        "complete-prefix",
        "-prefix",
        prefix,
        "-position",
        line ++ ":" ++ col,
        "-filename",
        fileName,
      |],
    );

  let ret = _parse(output);
  switch (ret) {
  | Error(v) => Error(v)
  | Return(json) =>
    json
    |> Protocol.completionResult_of_yojson
    |> LspProtocol.Utility.getResultOrThrow
    |> ((j: Protocol.completionResult) => j.entries)
    |> Ok
  };
};

let stopServer = (merlin: t) => {
  switch (merlin.mode) {
  | Server =>
    let _ = _run(merlin, [|"stop-server"|]);
    ();
  | Single => ()
  };
};
