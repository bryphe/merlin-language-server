module Notification = Notification;
module Types = Types;

[@deriving yojson({strict: false})]
type initializeParams = {rootUri: string};

[@deriving yojson({strict: false})]
type debugEchoParams = {message: string};

type request =
  | DebugEcho(Types.debugEchoParams)
  | Initialize(initializeParams)
  | TextDocumentHover(Types.textDocumentPositionParams)
  | TextDocumentCompletion(Types.textDocumentPositionParams)
  | UnknownRequest;

type t = request;

let is = (msg: Yojson.Safe.json) => Utility.hasMethod(msg) && Utility.hasId(msg);

let parse = (msg: Yojson.Safe.json) => {
  let method =
    msg |> Yojson.Safe.Util.member("method") |> Yojson.Safe.Util.to_string;

  let params = msg |> Yojson.Safe.Util.member("params");

  switch (method) {
  | "textDocument/completion" =>
    let v =
      Types.textDocumentPositionParams_of_yojson(params) |> Utility.getResultOrThrow;
    TextDocumentCompletion(v);
  | "textDocument/hover" =>
    let v =
      Types.textDocumentPositionParams_of_yojson(params) |> Utility.getResultOrThrow;
    TextDocumentHover(v);
  | "initialize" =>
    let v = initializeParams_of_yojson(params) |> Utility.getResultOrThrow;
    Initialize(v);
  | "debug/echo" =>
    let v = Types.debugEchoParams_of_yojson(params) |> Utility.getResultOrThrow;
    DebugEcho(v);
  | _ => UnknownRequest
  };
};
