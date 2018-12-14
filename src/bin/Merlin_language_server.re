module DocumentStore = Server.DocumentStore;
module Merlin = Server.Merlin;

let documentStore = DocumentStore.create();
let merlin: ref(option(Merlin.t)) = ref(None);

let onNotification = (notification: Protocol.Notification.t, rpc) =>
  switch (notification) {
  | TextDocumentDidOpen(v) => DocumentStore.openDocument(documentStore, v);
  | TextDocumentDidChange(v) => DocumentStore.changeDocument(documentStore, v);
  | Exit => Protocol.Rpc.stop(rpc)
  | _ => prerr_endline("Unhandled notification!")
  };

let initializeInfo: Protocol.Response.initializeResult = {
  capabilities: {
    textDocumentSync: 0,
  },
};

let initializeMerlin = () => {
   /* First, check if the MLS_MERLIN_PATH environment variable is set */
    
   let merlinPath = Rench.Environment.getEnvironmentVariable("MLS_MERLIN_PATH");
   switch (merlinPath) {
   | Some(x) => Merlin.init(merlinPath);
   | None => Merlin.init("ocamlmerlin");
   }
};

let onRequest = (_rpc, request: Protocol.Request.t) => {
  switch (request) {
  | TextDocumentCompletion(_) =>
    Protocol.Response.completionList_to_yojson({
      isIncomplete: false,
      items: [{label: "item1", detail: "item1 details"}],
    })
  | TextDocumentHover(_) =>
    let response = Merlin.getTypeEnclosing(merlin, 
    Protocol.Response.hover_to_yojson({contents: "Hello World!"})
  | Initialize(_p) =>
    merlin := Some(initializeMerlin());
    Protocol.Response.initializeResult_to_yojson(initializeInfo)
  | Shutdown =>
    switch (merlin^) {
    | Some(v) => Merlin.shutdown(v)
    | None => ()
    };
    `Null
  | DebugEcho(msg) => Protocol.Types.debugEchoParams_to_yojson(msg)
  | DebugTextDocumentGet(f) => 
    let doc = DocumentStore.getDocument(documentStore, f.textDocument.uri)
    switch (doc) {
    | Some(x) => `String(x.text)
    | None => `Null
    }
  | _ => `Null
  };
};

Protocol.Rpc.start(~onNotification, ~onRequest, stdin, stdout);

prerr_endline("Closing");
