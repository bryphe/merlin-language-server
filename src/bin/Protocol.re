
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

let parse: string => message = (_msg) => {
    /* let json = Yojson.Safe.from_string(msg); */
    Notification(Exit);
};
