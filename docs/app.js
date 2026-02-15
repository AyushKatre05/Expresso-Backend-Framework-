const output = document.getElementById('output');
const input = document.getElementById('cmdInput');
const promptElement = document.querySelector('.prompt');
let promptStr = 'root@expresso:~$ ';
let commandHistory = [];
let historyIndex = -1;
let currentDir = '/';

// Define Nano Editor
const nanoOverlay = document.createElement('div');
nanoOverlay.className = 'nano-overlay';
nanoOverlay.innerHTML = `
    <div class="nano-header">
        <span>GNU nano 5.4</span>
        <span id="nanoFileName">New Buffer</span>
        <span>Modified</span>
    </div>
    <textarea class="nano-content" id="nanoArea" spellcheck="false"></textarea>
    <div class="nano-footer">
        <span><span class="shortcut">^X</span> Exit</span>
        <span><span class="shortcut">^O</span> Write Out</span>
    </div>
`;
document.body.appendChild(nanoOverlay);
const nanoArea = document.getElementById('nanoArea');
const nanoFileName = document.getElementById('nanoFileName');
let isEditorOpen = false;

// Matrix Rain Effect
const canvas = document.createElement('canvas');
const ctx = canvas.getContext('2d');
let matrixInterval;

function startMatrix() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    canvas.style.position = 'fixed';
    canvas.style.top = '0';
    canvas.style.left = '0';
    canvas.style.zIndex = '0';
    canvas.style.opacity = '0.1';
    canvas.style.pointerEvents = 'none';
    document.body.appendChild(canvas);

    const alphabet = 'アァカサタナハマヤャラワガザダバパイィキシチニヒミリヰギジヂビピ0123456789';
    const fontSize = 16;
    const columns = canvas.width / fontSize;
    const drops = Array(Math.floor(columns)).fill(1);

    const draw = () => {
        ctx.fillStyle = 'rgba(0, 0, 0, 0.05)';
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        ctx.fillStyle = '#0F0';
        ctx.font = fontSize + 'px monospace';
        for(let i = 0; i < drops.length; i++) {
            const text = alphabet.charAt(Math.floor(Math.random() * alphabet.length));
            ctx.fillText(text, i*fontSize, drops[i]*fontSize);
            if(drops[i]*fontSize > canvas.height && Math.random() > 0.975) drops[i] = 0;
            drops[i]++;
        }
    };
    matrixInterval = setInterval(draw, 30);
    printLine('Matrix Protocol Initiated...', 'success');
}

// Input Handling
input.addEventListener('keydown', async (e) => {
    if (e.key === 'Enter') {
        const cmd = input.value.trim();
        if (cmd) {
            commandHistory.push(cmd);
            historyIndex = commandHistory.length;
            printLine(promptStr + cmd, 'command-echo');
            await processCommand(cmd);
        }
        input.value = '';
        window.scrollTo(0, document.body.scrollHeight);
    } else if (e.key === 'ArrowUp') {
        if (historyIndex > 0) {
            historyIndex--;
            input.value = commandHistory[historyIndex];
        }
    } else if (e.key === 'ArrowDown') {
        if (historyIndex < commandHistory.length - 1) {
            historyIndex++;
            input.value = commandHistory[historyIndex];
        } else {
            historyIndex = commandHistory.length;
            input.value = '';
        }
    }
});

// Nano Editor Keys
nanoArea.addEventListener('keydown', async (e) => {
    if (e.ctrlKey && e.key === 'x') { e.preventDefault(); closeNano(); }
    else if (e.ctrlKey && e.key === 'o') { e.preventDefault(); await saveNano(); }
});

function openNano(filename, content = '') {
    isEditorOpen = true;
    nanoOverlay.style.display = 'flex';
    nanoOverlay.dataset.filename = filename;
    nanoFileName.textContent = filename;
    nanoArea.value = content;
    nanoArea.focus();
}

function closeNano() {
    isEditorOpen = false;
    nanoOverlay.style.display = 'none';
    input.focus();
}

async function saveNano() {
    const filename = nanoOverlay.dataset.filename;
    const content = nanoArea.value;
    try {
        const res = await fetch(`/files/${filename}`, { method: 'POST', body: content });
        if (res.ok) printLine(`[SYSTEM] Successfully wrote to ${filename}`, 'success');
        else printLine('Error writing file', 'error');
    } catch(e) { printLine('Network Error', 'error'); }
}

document.addEventListener('click', () => { if (!isEditorOpen) input.focus(); });

function printLine(text, className = '') {
    const div = document.createElement('div');
    div.className = className || 'command-output';
    div.textContent = text;
    output.appendChild(div);
}

// Intercept fetch to show C++ Engine status
const originalFetch = window.fetch;
window.fetch = async (url, options) => {
    try {
        const response = await originalFetch(url, options);
        // We only care about X-Expresso-Cpp for API or file calls
        const cppHeader = response.headers.get('X-Expresso-Cpp');
        if (cppHeader) {
            printLine(`[SYSTEM] ${cppHeader}`, 'info-msg');
        }
        return response;
    } catch (e) {
        return originalFetch(url, options);
    }
};

// THE COMMAND PROCESSOR
async function processCommand(cmdLine) {
    const args = cmdLine.split(/\s+/).filter(a => a);
    if (args.length === 0) return;
    const cmd = args[0].toLowerCase();

    switch (cmd) {
        // --- CORE ---
        case 'help': 
            printLine('--- CORE COMMANDS ---');
            printLine('ls, cd, pwd, clear, history, man, exit, status, whoami, uptime, date, sysinfo, theme, matrix');
            printLine('--- FILE SYSTEM ---');
            printLine('cat, nano, touch, rm, mkdir, cp, mv, head, tail, wc, du, chmod, grep, stat, find, tree');
            printLine('--- NETWORK ---');
            printLine('ping, curl, wget, ip, ifconfig, netstat, uname, hostname, dig, nslookup, route, ssh, telnet');
            printLine('--- UTILS & FUN ---');
            printLine('hack, cowsay, fortune, banner, echo, cal, bc, sudo, df, free, ps, top, kill, alias, which');
            break;
        case 'man': window.open('manual.html', '_blank'); printLine('Manual documentation opened in new tab.'); break;
        case 'ls':
            try {
                const res = await fetch('/files');
                const text = await res.text();
                const files = text.split('\n').filter(f => f);
                printLine(files.length > 0 ? files.join('   ') : '(empty directory)');
            } catch(e) { printLine('File system error.', 'error'); }
            break;
        case 'cd':
            const dir = args[1] || '/';
            if (dir === '..') currentDir = '/';
            else if (dir === '/') currentDir = '/';
            else currentDir = dir.startsWith('/') ? dir : (currentDir === '/' ? `/${dir}` : `${currentDir}/${dir}`);
            promptStr = `root@expresso:${currentDir}$ `;
            if(promptElement) promptElement.textContent = promptStr;
            break;
        case 'pwd': printLine(currentDir); break;
        case 'clear': 
            output.innerHTML = ''; 
            printLine('Terminal cleared. Type "help" for a refresher.', 'info-msg');
            break;
        case 'history': printLine(commandHistory.map((c, i) => `${i+1} ${c}`).join('\n')); break;
        case 'whoami': printLine('root'); break;
        case 'uptime': printLine(`up ${Math.floor(performance.now()/60000)}m, 1 user, load average: 0.05, 0.03, 0.01`); break;
        case 'exit': printLine('Logout successful. Reloading...'); setTimeout(() => window.location.reload(), 1000); break;
        case 'date': printLine(new Date().toString()); break;

        // --- FILE SYSTEM ---
        case 'cat':
            if (!args[1]) return printLine('Usage: cat <filename>', 'error');
            try {
                const res = await fetch(`/files/${args[1]}`);
                if(res.ok) printLine(await res.text());
                else printLine(`cat: ${args[1]}: No such file`, 'error');
            } catch(e) { printLine('Network Error', 'error'); }
            break;
        case 'nano':
            if (!args[1]) return printLine('Usage: nano <filename>', 'error');
            let content = '';
            try { const r = await fetch(`/files/${args[1]}`); if(r.ok) content = await r.text(); } catch(e){}
            openNano(args[1], content);
            break;
        case 'touch': 
            if(!args[1]) return printLine('touch: missing operand', 'error');
            await fetch(`/files/${args[1]}`, {method:'POST', body:''}); 
            printLine(`Created/Updated ${args[1]}`);
            break;
        case 'rm': 
            if(!args[1]) return printLine('rm: missing operand', 'error');
            const rmRes = await fetch(`/files/${args[1]}`, {method:'DELETE'}); 
            if(rmRes.ok) printLine(`Removed ${args[1]}`);
            else printLine(`rm: cannot remove '${args[1]}': No such file`, 'error');
            break;
        case 'mkdir': 
            if(!args[1]) return printLine('mkdir: missing operand', 'error');
            const mkRes = await fetch(`/mkdir/${args[1]}`, {method:'POST'}); 
            if(mkRes.ok) printLine(`Created directory ${args[1]}`);
            else printLine(`mkdir: error creating '${args[1]}'`, 'error');
            break;
        case 'cp':
        case 'mv':
             if (args.length < 3) return printLine(`Usage: ${cmd} <src> <dst>`, 'error');
             try {
                const r = await fetch(`/files/${args[1]}`);
                if(!r.ok) throw new Error(`${cmd}: ${args[1]}: No such file`);
                const b = await r.blob();
                const pr = await fetch(`/files/${args[2]}`, {method:'POST', body:b});
                if(!pr.ok) throw new Error(`${cmd}: failed to create destination`);
                if(cmd === 'mv') await fetch(`/files/${args[1]}`, {method:'DELETE'});
                printLine(`${cmd === 'cp' ? 'Copied' : 'Moved'} successfully.`);
             } catch(e){ printLine(e.message, 'error'); }
             break;
        case 'head': 
        case 'tail': printLine('Buffer stream processing simulation active.'); break;
        case 'wc': printLine(`  42  210  1500 ${args[1]||''}`); break;
        case 'du': printLine(`4.0K\t${args[1]||'.'}`); break;
        case 'chmod': printLine(`Mode of '${args[1]||'.'}' changed to ${args[2]||'644'}`); break;
        case 'grep': printLine(`Searching for pattern in buffer...`); break;
        case 'stat': printLine(`  File: ${args[1]||'.'}\n  Size: 4096 \t Blocks: 8 \t IO Block: 4096 \nDevice: fd00h/64768d\tInode: 123456\tLinks: 1`); break;
        case 'find': printLine(`${currentDir}\n${currentDir}/app.js\n${currentDir}/index.html`); break;
        case 'tree': printLine(`.\n├── app.js\n├── index.html\n└── styles.css`); break;

        // --- NETWORK & SYSTEM ---
        case 'status':
            const stRes = await fetch('/user-agent');
            printLine(`[OFFICIAL STATUS] EXPRESO HYBRID CORE ACTIVE`, 'success');
            printLine(`[SYSTEM] Rust Kernel: 1.75.0 (V-FS ENABLED)`);
            printLine(`[SYSTEM] C++ Engine: V-LOGIC 2.4 (FFI HOOK ACTIVE)`);
            printLine(`[SYSTEM] C Parser: Legacy v1.0 (SECURE_STRTOK)`);
            break;
        case 'sysinfo': printLine('CPU: Expresso Optimized Virtual Core\nRAM: 16384 MB\nDisk: 250 GB V-SSD\nKernel: Rust-Native 5.15.0'); break;
        case 'ps': printLine('  PID TTY          TIME CMD\n    1 ?        00:00:01 expresso-rs\n   42 pts/0    00:00:00 sh'); break;
        case 'top': printLine('Tasks: 1 total, 0 running, 1 sleeping\n%Cpu(s): 0.5 us, 0.2 sy, 0.0 ni, 99.3 id'); break;
        case 'kill': printLine(`Process ${args[1] || '0'} terminated.`); break;
        case 'free': printLine('              total        used        free      shared  buff/cache   available\nMem:       16384000     4000000    12384000           0           0    12384000'); break;
        case 'df': printLine('Filesystem     1K-blocks      Used Available Use% Mounted on\n/dev/sda1      104857600  20480000  84377600  20% /'); break;
        case 'ping': printLine(`PING ${args[1] || 'localhost'} (127.0.0.1) 56(84) bytes of data.\n64 bytes from 127.0.0.1: icmp_seq=1 ttl=64 time=0.032 ms`); break;
        case 'curl':
        case 'wget': printLine(`Connecting to ${args[1] || 'expresso.dev'} (104.21.72.10)... connected.\nHTTP request sent, awaiting response... 200 OK`); break;
        case 'ip': printLine('1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default \n    inet 127.0.0.1/8 scope host lo'); break;
        case 'ifconfig': printLine('eth0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500\n        inet 172.17.0.2  netmask 255.255.0.0'); break;
        case 'netstat': printLine('Active Internet connections (w/o servers)\nProto Recv-Q Send-Q Local Address           Foreign Address         State'); break;
        case 'uname': printLine('Expresso Linux x86_64 Rust/C++ Hybrid'); break;
        case 'hostname': printLine('expresso-node-01'); break;
        case 'dig':
        case 'nslookup': printLine('; <<>> DiG 9.16.1-Ubuntu <<>> ' + (args[1]||'expresso.dev')); break;
        case 'route': printLine('Kernel IP routing table\nDestination     Gateway         Genmask         Flags Metric Ref    Use Iface\ndefault         172.17.0.1      0.0.0.0         UG    0      0        0 eth0'); break;
        case 'ssh': printLine(`ssh: connect to host ${args[1]||'remote'} port 22: Connection refused`); break;
        case 'telnet': printLine(`Trying ${args[1]||'127.0.0.1'}...\ntelnet: Unable to connect to remote host: Connection refused`); break;

        // --- UTILS & FUN ---
        case 'matrix': if(matrixInterval) { clearInterval(matrixInterval); matrixInterval=null; canvas.remove(); printLine('Matrix terminated.'); } else startMatrix(); break;
        case 'hack': printLine('Bypassing firewall...', 'error'); setTimeout(()=>printLine('Mainframe accessed. Welcome, Operator.', 'success'), 1200); break;
        case 'cowsay': 
            const msg = args.slice(1).join(' ') || 'Expresso is cool!';
            printLine(` __________________\n< ${msg} >\n ------------------\n        \\   ^__^\n         \\  (oo)\\_______\n            (__)\\       )\\/\\\n                ||----w |\n                ||     ||`); 
            break;
        case 'fortune': 
            const quotes = ["Code is like humor. When you have to explain it, it’s bad.", "First, solve the problem. Then, write the code.", "Experience is the name everyone gives to their mistakes."];
            printLine(quotes[Math.floor(Math.random()*quotes.length)]);
            break;
        case 'banner': printLine(`####################################\n#  ${args.slice(1).join(' ').toUpperCase() || 'EXPRESSO OS'}  #\n####################################`); break;
        case 'echo': printLine(args.slice(1).join(' ')); break;
        case 'cal': printLine('      February 2026\nSu Mo Tu We Th Fr Sa\n 1  2  3  4  5  6  7\n 8  9 10 11 12 13 14\n15 16 17 18 19 20 21\n22 23 24 25 26 27 28'); break;
        case 'bc': try { printLine('calc: ' + eval(args.slice(1).join(' '))); } catch(e){ printLine('Error'); } break;
        case 'sudo': printLine('root is not in the sudoers file. This incident will be reported.'); break;
        case 'alias': printLine(`alias ll='ls -la'\nalias cls='clear'`); break;
        case 'which': printLine(`/usr/bin/${args[1] || 'sh'}`); break;

        default: printLine(`expresso: command not found: ${cmd}. Type 'help' for support.`, 'error');
    }
}
async function saveNano() {
    const filename = nanoOverlay.dataset.filename;
    const content = nanoArea.value;
    try {
        const res = await fetch(`/files/${filename}`, { method: 'POST', body: content });
        if (res.ok) printLine(`[SYSTEM] Successfully wrote to ${filename}`, 'success');
        else printLine('Error writing file', 'error');
    } catch(e) { printLine('Network Error', 'error'); }
}
