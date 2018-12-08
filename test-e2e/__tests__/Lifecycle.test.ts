import * as LanguageServer from "./../src/LanguageServer"

import * as Protocol from "vscode-languageserver-protocol"

test('basic', async () => {
    let languageServer = LanguageServer.start();
    await LanguageServer.exit(languageServer);
});

test('initialize with empty capabilities', async () => {
    let languageServer = LanguageServer.start();

    let capabilities: Protocol.ClientCapabilities = {
           
    };

    let initializeParameters: Protocol.InitializeParams = {
        processId: process.pid,
        rootUri: process.cwd(),
        capabilities: capabilities,
        workspaceFolders: [],
    };

    let result = await languageServer.sendRequest(Protocol.InitializeRequest.type, initializeParameters);

    expect(result.capabilities).toBeTruthy();
    await LanguageServer.exit(languageServer);
});
