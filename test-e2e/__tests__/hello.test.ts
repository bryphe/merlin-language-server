import * as cp from 'child_process';
import * as path from 'path';
import * as rpc from 'vscode-jsonrpc'

test('basic', () => {
    expect(1).toBe(1);

    let serverPath = path.join(__dirname, "..", "..", "_build", "install", "default", "bin", "Hello.exe");

    let childProcess = cp.spawn(serverPath);

    let connection = rpc.createMessageConnection(
        new rpc.StreamMessageReader(childProcess.stdout),
        new rpc.StreamMessageWriter(childProcess.stdin)
    );

    let ret = new Promise((res, rej) => {
        childProcess.stderr.on("data", (d) => {
            console.log("Received data: " + d);
            res();
        })
    });

    let notification = new rpc.NotificationType<string, void>('testNotification');

    connection.listen();

    connection.sendNotification(notification, 'Hello World');

    return ret;

});
