type t = {
  input: in_channel,
  output: out_channel,
  mutable shouldClose: bool,
};

let start = (~onNotification, input: in_channel, output: out_channel) => {
  let rpc: t = {input, output, shouldClose: false};

  set_binary_mode_in(input, true);
  set_binary_mode_out(output, true);
  let id = Unix.descr_of_in_channel(stdin);
  while (!rpc.shouldClose) {
    Thread.wait_read(id);
    let contentLength = input_line(input);
    /* let contentType = input_line(stdin); */

    let preambleLength = String.length("Content-Length: ");
    let postLength = String.length("\r\n");
    /* let postLength = String.length("\r\n"); */
    /* let getContentLength = String.sub(contentLength, preambleLength, String.length(contentLength) - postLength - preambleLength + 1); */
    let getContentLength =
      String.sub(
        contentLength,
        preambleLength,
        String.length(contentLength) - postLength - preambleLength + 1,
      );
    prerr_endline("|" ++ contentLength ++ "|");
    prerr_endline("|" ++ getContentLength ++ "|");
    let len = int_of_string(getContentLength);

    let _ = input_line(input);

    /* Read message */

    let buffer = Bytes.create(len);
    let _ = Pervasives.input(input, buffer, 0, len);

    let str = Bytes.to_string(buffer);
    prerr_endline("Received msg: " ++ str);

    let result = Protocol.parse(str);

    switch (result) {
    | Notification(v) => onNotification(v, rpc)
    | _ => prerr_endline("Unhandled message")
    };
  };
};

let stop = (rpc: t) => rpc.shouldClose = true;
