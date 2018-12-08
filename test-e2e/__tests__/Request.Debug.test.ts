import * as LanguageServer from "./../src/LanguageServer"

import * as Protocol from "vscode-languageserver-protocol"

test('debug/echo', async () => {
    let languageServer = LanguageServer.start();

    let params = {
        message: "testing",
    };

    let result: any = await languageServer.sendRequest("debug/echo", params);

    expect(result.message).toBe("testing");
    await LanguageServer.exit(languageServer);
});
