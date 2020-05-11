const https = require('https');
const fs = require('fs');

const options = {
  key: fs.readFileSync('CERTIFICATES/key.pem'),
  cert: fs.readFileSync('CERTIFICATES/cert.pem')
};

https.createServer(options, function (req, res) {
  res.writeHead(200);
  res.end(fs.readFileSync('index.html'));
}).listen(8000);
