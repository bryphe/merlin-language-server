module Notification = Notification;
module Request = Request;
module Response = Response;
module Types = Types;

[@deriving yojson({strict: false})]
type hover = {contents: string};

[@deriving yojson({strict: false})]
type completionItem = {
  label: string,
  detail: string,
};

[@deriving yojson({strict: false})]
type completionList = {
  isIncomplete: bool,
  items: list(completionItem),
};

type response =
  | UnknownResponse;

type message =
  | Request(int, Request.t)
  | Notification(Notification.t)
  | Response(response);

let hasField = (f: string, msg: Yojson.Safe.json) => {
  let v = Yojson.Safe.Util.member(f, msg);

  switch (v) {
  | `Null => false
  | _ => true
  };
};

let hasMethod = hasField("method");
let hasId = hasField("id");
let hasResult = hasField("result");

let isResponse = (msg: Yojson.Safe.json) => hasResult(msg) && hasId(msg);

exception ParseException(string);

let getOrThrow = (r: Result.result('t, string)) =>
  switch (r) {
  | Ok(v) => v
  | Error(e) => raise(ParseException("getOrThrow: Error parsing: " ++ e))
  };

let parse: string => message =
  msg => {
    let p = Yojson.Safe.from_string(msg);

    switch (Notification.is(p), Request.is(p), isResponse(p)) {
    | (true, _, _) =>
      let n = Notification.parse(p);
      Notification(n);
    | (_, true, _) =>
      let id = p |> Yojson.Safe.Util.member("id") |> Yojson.Safe.Util.to_int;
      Request(id, Request.parse(p));
    | _ => Response(UnknownResponse)
    };
  };
