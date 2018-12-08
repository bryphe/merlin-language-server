
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
| Initialize(initializeRequestParams);

type notification =
| Exit;

type response =
| InitializeResult(initializeResult);

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

let parse: string => message = (_msg) => {
    /* let json = Yojson.Safe.from_string(msg); */
    Notification(Exit);
};
