
[@deriving yojson {strict: false}]
type requestMessage('t) = {
    id: int,
    method: string,
    params: 't,
};

[@deriving yojson {strict: false}]
type responseMessage('t) = {
    id: int,
    result: 't,
};

[@deriving yojson {strict: false}]
type initializeRequestParams = {
   id: int,
   processId: int,
   rootUri: string,
};

[@deriving yojson {strict: false}]
type initializeRequest = requestMessage(initializeRequestParams);

[@deriving yojson {strict: false}]
type serverCapabilities = {
    textDocumentSync: int
};

[@deriving yojson {strict: false}]
type initializeResult = {
    capabilities: serverCapabilities,
};

[@deriving yojson {strict: false}]
type initializeResponse = responseMessage(initializeResult);

type request = 
| Initialize(initializeRequestParams)
| UnknownRequest;

type notification =
| Exit;

type response =
| InitializeResult(initializeResult)
| UnknownResponse;

type message =
| Request(request)
| Notification(notification)
| Response(response);

let hasField = (f: string, msg: Yojson.Safe.json) => {
       let v = Yojson.Safe.Util.member(f, msg);

       switch (v) {
       | `Null => false
       | _ => true
       };
}

let hasMethod = hasField("method");
let hasId = hasField("id");
let hasResult = hasField("result");

let isNotification = (msg: Yojson.Safe.json) => {
    hasMethod(msg) && !hasId(msg);
};

let isRequest = (msg: Yojson.Safe.json) => {
    hasMethod(msg) && hasId(msg);
};

let isResponse = (msg: Yojson.Safe.json) => {
    hasResult(msg) && hasId(msg);    
};

exception ParseException(string);

let getOrThrow = (r: Result.result('t, string)) => {
    switch(r) {
    | Ok(v) => v
    | _ => raise(ParseException("Error parsing!"))
    }
};

let parseRequest = (msg: Yojson.Safe.json) => {

    let method = msg |> Yojson.Safe.Util.member("method") |> Yojson.Safe.Util.to_string;

    switch (method) {
    | "initialize" => 
        let v =initializeRequest_of_yojson(msg) |> getOrThrow;
        Initialize(v.params);
    | _ => UnknownRequest
    };
};

let parse: string => message = (msg) => {

    let p = Yojson.Safe.from_string(msg);

    switch ((isNotification(p), isRequest(p), isResponse(p))) {
    | (true, _, _) => Notification(Exit)
    | (_, true, _) => Request(parseRequest(p))
    | _ => Response(UnknownResponse)
    };
};
