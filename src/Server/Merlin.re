open Rench;

module LspProtocol = Protocol;

type t = {merlinPath: string};

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

    type response =
    | Return(Yojson.Safe.json)
    | Error(string);
};

let init = (merlinPath: string) => {
  let ret: t = {merlinPath: merlinPath};
  ret;
};

let getValueAsString = (json: Yojson.Safe.json) => {
    json |> Yojson.Safe.Util.member("value") |> Yojson.Safe.Util.to_string;
}

let _parse = (json: Yojson.Safe.json) => {
    let responseClass = json |> Yojson.Safe.Util.member("class") |> Yojson.Safe.Util.to_string;

    let isFailure = String.equal(responseClass, "error") || String.equal(responseClass, "failure") || String.equal(responseClass, "exception");
    let ret: Protocol.response = switch (isFailure) {
    | true => getValueAsString(json) |> Error;
    | false => 
        json |> Yojson.Safe.Util.member("value") |> Return;
    };
    ret;
};

let _run = (~input: string, merlin: t, command: array(string)) => {
  let opts = ChildProcess.SpawnSyncOptions.create(~input, ());
  let proc =
    ChildProcess.spawnSync(
      ~opts,
      merlin.merlinPath,
      Array.append([|"single"|], command),
    );

  proc.stdout |> Yojson.Safe.from_string;
};

let getTypeEnclosing = (merlin: t, position: Position.t, fileName: string, fileContents: string) => {
    let line = string_of_int(position.line);
    let col = string_of_int(position.col);
    let output = _run(~input=fileContents, merlin, [|"type-enclosing", "-position", line ++ ":" ++ col, "-filename", fileName|]);

    let ret = _parse(output);
    switch (ret) {
    | Error(v) => Error(v)
    | Return(json) => {
        json |> Protocol.typeEnclosingResult_of_yojson |> LspProtocol.Utility.getResultOrThrow |> Ok
    }
    };
}
