const iceoryx = require('bindings')('iceoryx.node')
// const appArgs = process.argv.slice(2);

iceoryx.setIoxProcessName("server_test");

const server = new iceoryx.IceoryxServer();
server.listen();
