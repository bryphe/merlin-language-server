module DocumentStore = Server.DocumentStore;

let documentStore = DocumentStore.create();

let onNotification = (notification: Protocol.Notification.t, rpc) =>
  switch (notification) {
  | TextDocumentDidOpen(args) => DocumentStore.openDocument(documentStore, args.textDocument);
  | Exit => Protocol.Rpc.stop(rpc)
  | _ => prerr_endline("Unhandled notification!")
  };

let initializeInfo: Protocol.Response.initializeResult = {
  capabilities: {
    textDocumentSync: 0,
  },
};

let onRequest = (_rpc, request: Protocol.Request.t) => {
  switch (request) {
  | TextDocumentCompletion(_) =>
    Protocol.Response.completionList_to_yojson({
      isIncomplete: false,
      items: [{label: "item1", detail: "item1 details"}],
    })
  | TextDocumentHover(_) =>
    Protocol.Response.hover_to_yojson({contents: "Hello World!"})
  | Initialize(_p) =>
    Protocol.Response.initializeResult_to_yojson(initializeInfo)
  | DebugEcho(msg) => Protocol.Types.debugEchoParams_to_yojson(msg)
  | _ => `Null
  };
};

Protocol.Rpc.start(~onNotification, ~onRequest, stdin, stdout);

prerr_endline("Closing");
