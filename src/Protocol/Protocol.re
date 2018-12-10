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

[@deriving yojson({strict: false})]
type didOpenTextDocumentParams = {textDocument: Types.textDocumentItem};

[@deriving yojson({strict: false})]
type hover = {contents: string};

[@deriving yojson({strict: false})]
type completionItem = {
  label: string,
  detail: string,
};

[@deriving yojson({strict: false})]
type completionList = {
  isIncomplete: bool,
  items: list(completionItem),
};

type request =
  | DebugEcho(debugEchoParams)
  | Initialize(initializeParams)
  | TextDocumentHover(Types.textDocumentPositionParams)
  | TextDocumentCompletion(Types.textDocumentPositionParams)
  | UnknownRequest;

type response =
  | UnknownResponse;

type message =
  | Request(int, request)
  | Notification(Notification.t)
  | Response(response);

let hasField = (f: string, msg: Yojson.Safe.json) => {
  let v = Yojson.Safe.Util.member(f, msg);

  switch (v) {
  | `Null => false
  | _ => true
  };
};

let hasMethod = hasField("method");
let hasId = hasField("id");
let hasResult = hasField("result");

let isRequest = (msg: Yojson.Safe.json) => hasMethod(msg) && hasId(msg);

let isResponse = (msg: Yojson.Safe.json) => hasResult(msg) && hasId(msg);

exception ParseException(string);

let getOrThrow = (r: Result.result('t, string)) =>
  switch (r) {
  | Ok(v) => v
  | Error(e) => raise(ParseException("getOrThrow: Error parsing: " ++ e))
  };

let parseRequest = (msg: Yojson.Safe.json) => {
  let method =
    msg |> Yojson.Safe.Util.member("method") |> Yojson.Safe.Util.to_string;

  let params = msg |> Yojson.Safe.Util.member("params");

  prerr_endline("GOT METHOD: " ++ method);

  switch (method) {
  | "textDocument/completion" =>
    let v = Types.textDocumentPositionParams_of_yojson(params) |> getOrThrow;
    TextDocumentCompletion(v);
  | "textDocument/hover" =>
    let v = Types.textDocumentPositionParams_of_yojson(params) |> getOrThrow;
    TextDocumentHover(v);
  | "initialize" =>
    let v = initializeParams_of_yojson(params) |> getOrThrow;
    Initialize(v);
  | "debug/echo" =>
    let v = debugEchoParams_of_yojson(params) |> getOrThrow;
    DebugEcho(v);
  | _ => UnknownRequest
  };
};

let parse: string => message =
  msg => {
    let p = Yojson.Safe.from_string(msg);

    switch (Notification.is(p), isRequest(p), isResponse(p)) {
    | (true, _, _) =>
      let n = Notification.parse(p);
      Notification(n);
    | (_, true, _) =>
      let id = p |> Yojson.Safe.Util.member("id") |> Yojson.Safe.Util.to_int;
      prerr_endline("GOT ID: " ++ string_of_int(id));
      Request(id, parseRequest(p));
    | _ => Response(UnknownResponse)
    };
  };
