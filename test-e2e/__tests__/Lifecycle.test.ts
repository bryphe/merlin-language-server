
import * as LanguageServer from "./../src/LanguageServer"

test('basic', async () => {
    let languageServer = LanguageServer.start();
    await LanguageServer.exit(languageServer);
});
