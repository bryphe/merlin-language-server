/* let () = Lwt_main.run(Util.hello()); */

open Lib;

let onNotification = (notification: Protocol.Notification.t, rpc) =>
  switch (notification) {
  | TextDocumentDidOpen(args) => prerr_endline("Got open notification for: " ++ args.textDocument.uri);
  | Exit => Rpc.stop(rpc)
  | _ => prerr_endline("Unhandled notification!");
  /* | _ => prerr_endline ("Unknown notification!"); */
  };

let initializeInfo: Lib.Protocol.Response.initializeResult = {
    capabilities: {
        textDocumentSync: 0, 
    }
};

let onRequest = (_rpc, request: Lib.Protocol.Request.t) => {
    switch(request) {
    | TextDocumentCompletion(_) =>
        Lib.Protocol.completionList_to_yojson({ isIncomplete: false, items: [{label: "item1", detail: "item1 details"}]});
    | TextDocumentHover(_) => 
        Lib.Protocol.hover_to_yojson({contents: "Hello World!"});
    | Initialize(_p) => 
        Lib.Protocol.Response.initializeResult_to_yojson(initializeInfo);
    | DebugEcho(msg) => 
        Lib.Protocol.Types.debugEchoParams_to_yojson(msg);
    | _ => `Null;
    }
};

Rpc.start(~onNotification, ~onRequest, stdin, stdout);

prerr_endline("Closing");
