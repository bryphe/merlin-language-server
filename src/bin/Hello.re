/* let () = Lwt_main.run(Util.hello()); */


set_binary_mode_in(stdin, true);
set_binary_mode_out(stdout, true);

let id = Unix.descr_of_in_channel(stdin);

/* let ic = Unix.in_channel_of_descr(stdin); */

while (true) {
	Thread.wait_read(id);
	let contentLength = input_line(stdin);
	/* let contentType = input_line(stdin); */

	let preambleLength = String.length("Content-Length: ");
    /* let postLength = String.length("\r\n"); */
	/* let getContentLength = String.sub(contentLength, preambleLength, String.length(contentLength) - postLength - preambleLength + 1); */
	let getContentLength = String.sub(contentLength, preambleLength, 2);
    prerr_endline( "|" ++ getContentLength ++ "|");
 	let len = int_of_string(getContentLength);

	let _ = input_line(stdin);

    /* Read message */

	let buffer = Bytes.create(len);
	let t = input(stdin, buffer, 0, len);
    prerr_endline ("Got " ++ string_of_int(t) ++ "bytes, expected: " ++ string_of_int(len));

    let str = Bytes.to_string(buffer);

	prerr_endline ("Hello, world2:" ++ getContentLength ++ " | " ++ str);
}
