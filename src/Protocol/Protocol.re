module Notification = Notification;
module Request = Request;
module Response = Response;
module Types = Types;

type message =
  | Request(int, Request.t)
  | Notification(Notification.t)
  | Response(Response.t);

let parse: string => message =
  msg => {
    let p = Yojson.Safe.from_string(msg);

    switch (Notification.is(p), Request.is(p), Response.is(p)) {
    | (true, _, _) =>
      let n = Notification.parse(p);
      Notification(n);
    | (_, true, _) =>
      let id = p |> Yojson.Safe.Util.member("id") |> Yojson.Safe.Util.to_int;
      Request(id, Request.parse(p));
    | _ => Response(UnknownResponse)
    };
  };
