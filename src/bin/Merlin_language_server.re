module DocumentStore = Server.DocumentStore;
module Merlin = Server.Merlin;
module MerlinLspBridge = Server.MerlinLspBridge;

let documentStore = DocumentStore.create();
let merlin: ref(option(Merlin.t)) = ref(None);

let sendErrors = (rpc, uri: Protocol.Types.documentUri, diagnostics: Protocol.Types.diagnostics) => {
    let errorJson = Protocol.Types.diagnostics_to_yojson(diagnostics);
    let error = `Assoc([("uri", `String(uri)), ("diagnostics", errorJson)]);
    Protocol.Rpc.sendNotification(rpc, "textDocument/publishDiagnostics", error);
};

let checkErrors = (rpc, uri: Protocol.Types.documentUri) => {
    switch (merlin^) {
    | None => ()
    | Some(m) => 
        let errors = MerlinLspBridge.errors(m, documentStore, uri);
        switch (errors) {
        | Some(e) => sendErrors(rpc, uri, e)
        | None => ()
        }
    };
};
let onNotification = (notification: Protocol.Notification.t, rpc) =>
  switch (notification) {
  | TextDocumentDidOpen(v) => 
        DocumentStore.openDocument(documentStore, v);
        checkErrors(rpc, v.textDocument.uri);
  | TextDocumentDidChange(v) => 
        DocumentStore.changeDocument(documentStore, v);
        checkErrors(rpc, v.textDocument.uri);
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
   | Some(v) => Merlin.init(Merlin.Single, v);
   | None => Merlin.init(Merlin.Single, "C:\\Users\\bryph\\.esy\\3_\\i\\opam__s__merlin-opam__c__3.2.2-9e36d08c\\bin\\ocamlmerlin.exe");
   }
};

let onRequest = (_rpc, request: Protocol.Request.t) => {
  switch (request) {
  | TextDocumentCompletion(_) =>
    Protocol.Response.completionList_to_yojson({
      isIncomplete: false,
      items: [{label: "item1", detail: "item1 details"}],
    })
  | TextDocumentHover(tdp) =>
    switch (merlin^) {
    | None => `Null
    | Some(m) => {
        let hoverResult = MerlinLspBridge.hover(m, documentStore, tdp);
        switch (hoverResult) {
        | Some(v) => Protocol.Response.hover_to_yojson(v)
        | None => `Null
        }
        }
    }
  | Initialize(_p) =>
    merlin := Some(initializeMerlin());
    Protocol.Response.initializeResult_to_yojson(initializeInfo)
  | Shutdown =>
    switch (merlin^) {
    | Some(v) => Merlin.stopServer(v)
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
