/* let () = Lwt_main.run(Util.hello()); */

open Lib;

let onNotification = (notification: Lib.Protocol.notification, rpc) =>
  switch (notification) {
  | TextDocumentDidOpen(args) => prerr_endline("Got open notification for: " ++ args.textDocument.uri);
  | Exit => Rpc.stop(rpc)
  | _ => prerr_endline("Unhandled notification!");
  /* | _ => prerr_endline ("Unknown notification!"); */
  };

let initializeInfo: Lib.Protocol.initializeResult = {
    capabilities: {
        textDocumentSync: 0, 
    }
};


let onRequest = (_rpc, request: Lib.Protocol.request) => {
    switch(request) {
    | TextDocumentHover(_) => 
            Lib.Protocol.hover_to_yojson({contents: "Hello World!"});
    | Initialize(_p) => 
        Lib.Protocol.initializeResult_to_yojson(initializeInfo);
    | DebugEcho(msg) => 
        Lib.Protocol.debugEchoParams_to_yojson(msg);
    | _ => `Null;
    }
};

Rpc.start(~onNotification, ~onRequest, stdin, stdout);

prerr_endline("Closing");
