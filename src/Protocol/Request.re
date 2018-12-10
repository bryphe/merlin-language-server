module Notification = Notification;
module Types = Types;

[@deriving yojson({strict: false})]
type initializeParams = {rootUri: string};

[@deriving yojson({strict: false})]
type debugEchoParams = {message: string};

[@deriving yojson({strict: false})]
type serverCapabilities = {textDocumentSync: int};

[@deriving yojson({strict: false})]
type initializeResult = {capabilities: serverCapabilities};

type request =
  | DebugEcho(Types.debugEchoParams)
  | Initialize(initializeParams)
  | TextDocumentHover(Types.textDocumentPositionParams)
  | TextDocumentCompletion(Types.textDocumentPositionParams)
  | UnknownRequest;

let parse = (msg: Yojson.Safe.json) => {
  let method =
    msg |> Yojson.Safe.Util.member("method") |> Yojson.Safe.Util.to_string;

  let params = msg |> Yojson.Safe.Util.member("params");

  switch (method) {
  | "textDocument/completion" =>
    let v = 
        Types.textDocumentPositionParams_of_yojson(params) 
        |> Utility.getOrThrow;
    TextDocumentCompletion(v);
  | "textDocument/hover" =>
    let v = 
        Types.textDocumentPositionParams_of_yojson(params) 
        |> Utility.getOrThrow;
    TextDocumentHover(v);
  | "initialize" =>
    let v = 
        initializeParams_of_yojson(params) 
        |> getOrThrow;
    Initialize(v);
  | "debug/echo" =>
    let v = 
        debugEchoParams_of_yojson(params) 
        |> getOrThrow;
    DebugEcho(v);
  | _ => UnknownRequest
  };
};
