/*
 * Protocol
 *
 * Types and modules generally applicable to any language server
 */

module Log = Log;
module Notification = Notification;
module Preamble = Preamble;
module Request = Request;
module Response = Response;
module Rpc = Rpc;
module Types = Types;
module Utility = Utility;

type message =
  | Request(int, Request.t)
  | Notification(Notification.t)
  | Response(Response.t);

let parse: string => message =
  msg => {
    let p = Yojson.Safe.from_string(msg);

    switch (Notification.is(p), Request.is(p), Response.is(p)) {
    | (true, _, _) => Notification.parse(p) |> Notification
    | (_, true, _) =>
      let id = p |> Yojson.Safe.Util.member("id") |> Yojson.Safe.Util.to_int;
      Request(id, Request.parse(p));
    | _ => Response(UnknownResponse)
    };
  };
