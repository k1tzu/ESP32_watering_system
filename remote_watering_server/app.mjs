'use strict'
import express from 'express';
import cookiep from 'cookie-parser';
import session from 'express-session';
import bodyParser from 'body-parser';
import i18n from 'i18n';
import dgram from 'dgram';
import path from 'path';

const divider = "<!--insert from here-->";
const dividerLength = divider.length;
const sensorForms = [];
// const soilStateTail = ""
// const soilStateTailSize = soilStateTail.length;

const __dirname = path.resolve();

const i18nOptions = {
    directory: path.join(__dirname, "i18n"),
    locales: ['ru', 'en'],
    cookie: 'locale',
    defaultLocale: "ru"
};

i18n.configure(i18nOptions);

const app = express();
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(cookiep());
app.use(session({
    secret: 'secret', saveUninitialized: true,
    resave: true
}));


app.use((request, response, next) => {
    response.locals.session = request.session;
    next();
});

app.use(i18n.init);

app.get('/', (request, response) => {
    response.render('index', {
        currentLang: 'ru',
        sensors
    });
});

app.post('/submit', (request, response, next) => {
    console.log(request.body);
        const body = Object.keys(request.body)[0];
        sendCommandToDevice(body.split("_")[0], body.split("_")[1]);
        response.writeHead(200, {"Content-Type": "text/html"});
        response.end("<!DOCTYPE HTML>\n" +
            "<html lang=\"en-US\">\n" +
            "    <head>\n" +
            "        <meta charset=\"UTF-8\">\n" +
            "        <meta http-equiv=\"refresh\" content=\"2; url=/\">\n" +
            "        <script type=\"text/javascript\">\n" +
            "              window.setTimeout(window.history.back(),3000);\n" +
            "        </script>\n" +
            "        <title>Page Redirection</title>\n" +
            "    </head>\n" +
            "    <body>\n" +
            "Message Sent\n" +
            "    </body>\n" +
            "</html>");
    next();
});

app.listen(3000);

const sensors = {};
app.UDPserver = dgram.createSocket("udp4");
app.UDPserver.bind(64399, "0.0.0.0");
app.UDPserver.on("message", function (msg, rinfo) {

    //got stat on port
    msg = msg.toString();
    msg = msg.replace("ON", "\"ON\"");
    msg = msg.replace("OFF", "\"OFF\"");

    let message;
    try {
        message = JSON.parse(msg);
    } catch (e) {
        return;
    }

    const sensorID = message.sensor ? message.sensor : "Default";

    if (!sensors[sensorID]) {
        sensors[sensorID] = {};
        sensors[sensorID].sensorID = sensorID;
        sensors[sensorID].pumpColor = "gray";
        sensors[sensorID].pumpTimer = 0;
        sensors[sensorID].pumpState = "None";
        sensors[sensorID].soilColor = "gray";
        sensors[sensorID].soilState = "None";
        sensors[sensorID].soilValue = 0;
    }

    sensors[sensorID].ip = rinfo.address;
    sensors[sensorID].port = rinfo.port;
    sensors[sensorID].message = message;

    // createSensorDivIfNeeded(sensorID);

    if (message.pumpstate) {
        sensors[sensorID] = setPumpState(sensorID, message.pumpstate, message.pumpTimer);
    }

    if (message.drySoilValue === undefined) {
        message.drySoilValue = 60;
        message.wetSoilValue = 100;
    }

    if (message.mositure !== undefined) {
        sensors[sensorID] = setSoilState(sensorID, message.mositure,message.drySoilValue, message.wetSoilValue);
    } else {
        console.info("Moisture unknown for", sensorID);
    }

    // console.info("Got message:", msg);
});

function setPumpState(sensorID, pumpState, pumpTimer) {
    pumpTimer = secondsToString(Math.abs(pumpTimer));
    const pumpColor = pumpState == 'ON' ? "#ef5350" : "#1de8b5";
    return {
        ...sensors[sensorID],
        pumpState,
        pumpTimer,
        pumpColor,

    };
}

const secondsToString = (function () {
    const pad = t => {
        return (t + '').length < 2 ? pad('0' + t + '') : t;
    }

    return s => {

        const d = Math.floor(s / (3600 * 24));

        s -= d * 3600 * 24;

        const h = Math.floor(s / 3600);

        s -= h * 3600;

        const m = Math.floor(s / 60);

        s -= m * 60;

        const tmp = [];

        (d) && tmp.push(d + 'd');

        (d || h) && tmp.push(h + 'h');

        (d || h || m) && tmp.push(m + 'm');

        tmp.push(s + 's');

        return tmp.join(' ');
    }
}());

function setSoilState(sensorID, soilValue, drySoilValue, wetSoilValue) {
    let soilColor;
    let soilState;
    if (soilValue < drySoilValue) {
        soilState = "DRY";
        soilColor = "#D8C0A8";
    } else if (soilValue < wetSoilValue) {
        soilState = "MOIST";
        soilColor = "#B2906F";
    } else if (soilValue > wetSoilValue) {
        soilState = "WET";
        soilColor = "#3F250B";
    }

    return {
        ...sensors[sensorID],
        soilState,
        soilValue,
        soilColor,
    };
}

function sendCommandToDevice(serialID, command) {
    app.UDPserver.send(command, sensors[serialID].port, sensors[serialID].ip, function (error) {
        if (error) {
            console.error("Error sending to", serialID, sensors[serialID], error);
        } else {
            console.log(`Data ${command} sent to`, serialID);
        }

    });
}

