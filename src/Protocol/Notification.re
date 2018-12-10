open Types;

[@deriving yojson({strict: false})]
type didOpenTextDocumentParams = {textDocument: textDocumentItem};

type notification =
  | TextDocumentDidOpen(didOpenTextDocumentParams)
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
    let msg =
      didOpenTextDocumentParams_of_yojson(params) |> Utility.getResultOrThrow;
    TextDocumentDidOpen(msg);
  | "exit" => Exit
  | _ => UnknownNotification
  };
};
