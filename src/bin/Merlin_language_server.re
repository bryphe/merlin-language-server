module DocumentStore = Server.DocumentStore;
module Merlin = Server.Merlin;
module MerlinLspBridge = Server.MerlinLspBridge;

open Protocol.Ppx_let_syntax_result;

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

let getMerlin = () => {
    switch(merlin^) {
    | Some(m) => Ok(m)
    | None => Error("Unable to get merlin instance")
    };
}

let onRequest = (_rpc, request: Protocol.Request.t) => {
  switch (request) {
  | TextDocumentCompletion(tdp) =>
    let%bind merlin = getMerlin();
    let completionList = MerlinLspBridge.completion(merlin, documentStore, tdp);
    switch (completionList) {
    | None => Ok(`Null)
    | Some(completionList) => Ok(Protocol.Response.completionList_to_yojson(completionList))
    }
  | TextDocumentHover(tdp) =>
    let%bind merlin = getMerlin();
    let hoverResult = MerlinLspBridge.hover(merlin, documentStore, tdp);
    switch (hoverResult) {
    | Some(v) => Ok(Protocol.Response.hover_to_yojson(v))
    | None => Ok(`Null)
    }
  | Initialize(_p) =>
    merlin := Some(initializeMerlin());
    Ok(Protocol.Response.initializeResult_to_yojson(initializeInfo))
  | Shutdown =>
    let%bind merlin = getMerlin();
    Merlin.stopServer(merlin);
    Ok(`Null);
  | DebugEcho(msg) => Ok(Protocol.Types.debugEchoParams_to_yojson(msg))
  | DebugTextDocumentGet(f) => 
    let doc = DocumentStore.getDocument(documentStore, f.textDocument.uri)
    switch (doc) {
    | Some(x) => Ok(`String(x.text))
    | None => Ok(`Null)
    }
  | _ => Ok(`Null)
  };
};

Protocol.Rpc.start(~onNotification, ~onRequest, stdin, stdout);

prerr_endline("Closing");
