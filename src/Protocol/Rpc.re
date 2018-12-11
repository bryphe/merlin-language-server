type t = {
  input: in_channel,
  output: out_channel,
  mutable shouldClose: bool,
};

let _sendResponse = (rpc: t, msg: Yojson.Safe.json, id: int) => {
  let response = `Assoc([("id", `Int(id)), ("result", msg)]);
  let str = Yojson.Safe.to_string(response);

  let length = String.length(str);
  let contentLengthString =
    "Content-Length: " ++ string_of_int(length) ++ "\r\n";
  output_string(rpc.output, contentLengthString);
  output_string(rpc.output, "\r\n");
  output_string(rpc.output, str);
  flush(rpc.output);
};

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

let start =
    (~onNotification, ~onRequest, input: in_channel, output: out_channel) => {
  let rpc: t = {input, output, shouldClose: false};

  set_binary_mode_in(input, true);
  set_binary_mode_out(output, true);
  let id = Unix.descr_of_in_channel(stdin);
  while (!rpc.shouldClose) {
    Thread.wait_read(id);

    let preamble = Preamble.read(input);
    let len = preamble.contentLength;

    /* Read message */
    let buffer = Bytes.create(len);
    let _ = Pervasives.input(input, buffer, 0, len);

    let str = Bytes.to_string(buffer);
    prerr_endline("Received msg: " ++ str);

    let result = parse(str);

    switch (result) {
    | Notification(v) => onNotification(v, rpc)
    | Request(id, v) =>
      let result = onRequest(rpc, v);
      _sendResponse(rpc, result, id);
    | _ => prerr_endline("Unhandled message")
    };
  };
};

let stop = (rpc: t) => rpc.shouldClose = true;
