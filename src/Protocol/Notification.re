open Types;

[@deriving yojson({strict: false})]
type didOpenTextDocumentParams = {textDocument: textDocumentItem};

[@deriving yojson({strict: false})]
type didChangeTextDocumentParams = {
  textDocument: Types.versionedTextDocumentIdentifier,
  contentChanges: list(textDocumentContentChangeEvent),
};

type notification =
  | TextDocumentDidOpen(didOpenTextDocumentParams)
  | TextDocumentDidChange(didChangeTextDocumentParams)
  | Exit
  | UnknownNotification;

type t = notification;

let is = (msg: Yojson.Safe.json) =>
  Utility.hasMethod(msg) && !Utility.hasId(msg);

let parse = (msg: Yojson.Safe.json) => {
  let method =
    msg |> Yojson.Safe.Util.member("method") |> Yojson.Safe.Util.to_string;

  let params = msg |> Yojson.Safe.Util.member("params");

  switch (method) {
  | "textDocument/didOpen" =>
    didOpenTextDocumentParams_of_yojson(params)
    |> Utility.getResultOrThrow
    |> TextDocumentDidOpen
  | "textDocument/didChange" =>
    didChangeTextDocumentParams_of_yojson(params)
    |> Utility.getResultOrThrow
    |> TextDocumentDidChange
  | "exit" => Exit
  | _ => UnknownNotification
  };
};
