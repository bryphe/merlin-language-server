/* let () = Lwt_main.run(Util.hello()); */


set_binary_mode_in(stdin, true);
set_binary_mode_out(stdout, true);

let id = Unix.descr_of_in_channel(stdin);

/* let ic = Unix.in_channel_of_descr(stdin); */

 [@deriving yojson {strict: false} ]
type notification = {
    jsonrpc: string,   
    method: string,
}; 


let shouldClose = ref(false);

while (!shouldClose^) {
	Thread.wait_read(id);
	let contentLength = input_line(stdin);
	/* let contentType = input_line(stdin); */

	let preambleLength = String.length("Content-Length: ");
    let postLength = String.length("\r\n");
    /* let postLength = String.length("\r\n"); */
	/* let getContentLength = String.sub(contentLength, preambleLength, String.length(contentLength) - postLength - preambleLength + 1); */
	let getContentLength = String.sub(contentLength, preambleLength, String.length(contentLength) - postLength - preambleLength + 1);
    prerr_endline( "|" ++ contentLength ++ "|");
    prerr_endline( "|" ++ getContentLength ++ "|");
 	let len = int_of_string(getContentLength);

	let _ = input_line(stdin);

    /* Read message */

	let buffer = Bytes.create(len);
	let _ = input(stdin, buffer, 0, len);

    let str = Bytes.to_string(buffer);
    prerr_endline ("Received msg: " ++ str);

    let result = Protocol.parse(str);

    switch (result) {
    | Notification(Exit) => shouldClose := true
    | _ => prerr_endline ("Unhandled message");
    }
}

prerr_endline ("Closing");
