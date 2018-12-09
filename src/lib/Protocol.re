[@deriving yojson({strict: false})]
type initializeParams = {
  rootUri: string,
};

[@deriving yojson({strict: false})]
type debugEchoParams = {
   message: string 
};

[@deriving yojson({strict: false})]
type serverCapabilities = {textDocumentSync: int};

[@deriving yojson({strict: false})]
type initializeResult = {capabilities: serverCapabilities};

[@deriving yojson({strict: false})]
type documentUri = string;

[@deriving yojson({strict: false})]
type textDocumentItem = {
    uri: documentUri,
    languageId: string,
    /* version: int, */
    text: string,
};

[@deriving yojson({strict: false})]
type zeroBasedLine = int;

[@deriving yojson({strict: false})]
type zeroBasedCharacter = int;

[@deriving yojson({strict: false})]
type position = {
    line: zeroBasedLine,
    character: zeroBasedCharacter,
};

[@deriving yojson({strict: false})]
type textDocumentIdentifier = {
  uri: documentUri,
};

[@deriving yojson({strict: false})]
type textDocumentPositionParams = {
   textDocument: textDocumentIdentifier,
   position: position,
};

[@deriving yojson({strict: false})]
type didOpenTextDocumentParams = {
    textDocument: textDocumentItem, 
};

[@deriving yojson({strict: false})]
type hover = {
    contents: string
};

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
  | TextDocumentHover(textDocumentPositionParams)
  | TextDocumentCompletion(textDocumentPositionParams)
  | UnknownRequest;

type notification =
  | TextDocumentDidOpen(didOpenTextDocumentParams)
  | Exit
  | UnknownNotification;

type response =
  | UnknownResponse;

type message =
  | Request(int, request)
  | Notification(notification)
  | Response(response)

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

let isNotification = (msg: Yojson.Safe.json) =>
  hasMethod(msg) && !hasId(msg);

let isRequest = (msg: Yojson.Safe.json) => hasMethod(msg) && hasId(msg);

let isResponse = (msg: Yojson.Safe.json) => hasResult(msg) && hasId(msg);

exception ParseException(string);

let getOrThrow = (r: Result.result('t, string)) =>
  switch (r) {
  | Ok(v) => v
  | Error(e) => raise(ParseException("getOrThrow: Error parsing: " ++ e))
  };

let parseNotification = (msg:Yojson.Safe.json) => {
  let method =
    msg |> Yojson.Safe.Util.member("method") |> Yojson.Safe.Util.to_string;

  let params =
     msg |> Yojson.Safe.Util.member("params");

  switch(method) {
  | "textDocument/didOpen" =>
    let msg = didOpenTextDocumentParams_of_yojson(params) |> getOrThrow;
    TextDocumentDidOpen(msg);
  | "exit" => Exit
  | _ => UnknownNotification
  }
    
}

let parseRequest = (msg: Yojson.Safe.json) => {
  let method =
    msg |> Yojson.Safe.Util.member("method") |> Yojson.Safe.Util.to_string;

  let params = msg |> Yojson.Safe.Util.member("params");

  prerr_endline ("GOT METHOD: " ++ method);

  switch (method) {
  | "textDocument/completion" =>
    let v = textDocumentPositionParams_of_yojson(params) |> getOrThrow;
    TextDocumentCompletion(v);
  | "textDocument/hover" =>
    let v = textDocumentPositionParams_of_yojson(params) |> getOrThrow;
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

    switch (isNotification(p), isRequest(p), isResponse(p)) {
    | (true, _, _) => 
        let n = parseNotification(p);
        Notification(n);
    | (_, true, _) => 
        let id = p |> Yojson.Safe.Util.member("id") |> Yojson.Safe.Util.to_int;
        prerr_endline ("GOT ID: " ++ string_of_int(id));
        Request(id, parseRequest(p))
    | _ => Response(UnknownResponse)
    };
  };
