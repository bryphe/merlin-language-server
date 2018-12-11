import * as LanguageServer from "./../src/LanguageServer"

import * as Protocol from "vscode-languageserver-protocol"
import * as Types from "vscode-languageserver-types"

describe("TextDocument", () => {
    let languageServer: LanguageServer.LanguageServer;
    beforeEach(async () => {
        languageServer = await LanguageServer.startAndInitialize();
    });

    afterEach(async () => {
        await LanguageServer.exit(languageServer)
        languageServer = null;
    });

    describe("didOpen", () => {
        it("stores text document", async () => {
            await languageServer.sendNotification("textDocument/didOpen", {
                textDocument: Types.TextDocumentItem.create("file:///test-document.txt", "txt", 0, "Hello, World!")
            });

            let result = await languageServer.sendRequest("debug/textDocument/get", {
                textDocument: Types.TextDocumentIdentifier.create("file:///test-document.txt"),
                position: Types.Position.create(0, 0),
            });

            expect(result).toEqual("Hello, World!");
        });
    });
});
