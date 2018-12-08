/* let () = Lwt_main.run(Util.hello()); */

open Lib;

let onNotification = (notification: Lib.Protocol.notification, rpc) =>
  switch (notification) {
  | Exit => Rpc.stop(rpc)
  /* | _ => prerr_endline ("Unknown notification!"); */
  };

Rpc.start(~onNotification, stdin, stdout);

prerr_endline("Closing");
