module Notification = Notification;
module Types = Types;

[@deriving yojson({strict: false})]
type initializeParams = {rootUri: string};

[@deriving yojson({strict: false})]
type debugGetDocumentParams = {uri: Types.documentUri};

type request =
  /* LSP requests */
  | Initialize(initializeParams)
  | Shutdown
  | TextDocumentHover(Types.textDocumentPositionParams)
  | TextDocumentCompletion(Types.textDocumentPositionParams)
  /* Debug requests - internal use / debugging only */
  | DebugEcho(Types.debugEchoParams)
  | DebugTextDocumentGet(Types.textDocumentPositionParams)
  | UnknownRequest;

type t = request;

let is = (msg: Yojson.Safe.json) =>
  Utility.hasMethod(msg) && Utility.hasId(msg);

let parse = (msg: Yojson.Safe.json) => {
  let method =
    msg |> Yojson.Safe.Util.member("method") |> Yojson.Safe.Util.to_string;

  let params = msg |> Yojson.Safe.Util.member("params");

  switch (method) {
  | "shutdown" => Shutdown
  | "textDocument/completion" =>
    Types.textDocumentPositionParams_of_yojson(params)
    |> Utility.getResultOrThrow
    |> TextDocumentCompletion
  | "textDocument/hover" =>
    Types.textDocumentPositionParams_of_yojson(params)
    |> Utility.getResultOrThrow
    |> TextDocumentHover
  | "initialize" =>
    initializeParams_of_yojson(params)
    |> Utility.getResultOrThrow
    |> Initialize
  | "debug/echo" =>
    Types.debugEchoParams_of_yojson(params)
    |> Utility.getResultOrThrow
    |> DebugEcho
  | "debug/textDocument/get" =>
    Types.textDocumentPositionParams_of_yojson(params)
    |> Utility.getResultOrThrow
    |> DebugTextDocumentGet
  | _ => UnknownRequest
  };
};
