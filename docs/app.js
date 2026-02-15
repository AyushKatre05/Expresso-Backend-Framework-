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
        if (res.ok) printLine(`Successfully wrote to ${filename}`, 'success');
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

// THE COMMAND PROCESSOR
async function processCommand(cmdLine) {
    const args = cmdLine.split(/\s+/);
    const cmd = args[0].toLowerCase();

    // EVERY command sends a "hit" to the backend FFI (C++ Engine)
    // We do this by hitting a special /audit endpoint or just the normal one
    // But for this demo, every request to /files etc already triggers the CPP log.
    
    switch (cmd) {
        // --- NAVIGATION & CORE (10) ---
        case 'help': printLine('Available commands: ls, cd, pwd, clear, history, cat, rm, mkdir, touch, cp, mv, ping, curl, status, whoami, date, theme, matrix, man, exit, hack, cowsay, uptime, sysinfo...'); break;
        case 'man': window.open('/docs/manual.html', '_blank'); printLine('Opening manual...'); break;
        case 'ls':
            try {
                const res = await fetch('/files');
                const text = await res.text();
                printLine(text.split('\n').filter(f => f).join('  ') || '(empty)');
            } catch(e) { printLine('Error', 'error'); }
            break;
        case 'cd':
            const dir = args[1] || '/';
            if (dir === '..') currentDir = '/';
            else currentDir = dir.startsWith('/') ? dir : (currentDir === '/' ? `/${dir}` : `${currentDir}/${dir}`);
            promptStr = `root@expresso:${currentDir}$ `;
            if(promptElement) promptElement.textContent = promptStr;
            break;
        case 'pwd': printLine(currentDir); break;
        case 'clear': output.innerHTML = ''; break;
        case 'history': printLine(commandHistory.join('\n')); break;
        case 'whoami': printLine('root'); break;
        case 'uptime': printLine(`up ${Math.floor(performance.now()/60000)}m, 1 user`); break;
        case 'exit': window.location.reload(); break;
        case 'date': printLine(new Date().toLocaleString()); break;

        // --- FILE OPS (15) ---
        case 'cat':
            if (!args[1]) return printLine('cat <file>');
            try {
                const res = await fetch(`/files/${args[1]}`);
                printLine(await res.text());
            } catch(e) { printLine('Not found'); }
            break;
        case 'nano':
            if (!args[1]) return printLine('nano <file>');
            let content = '';
            try { const r = await fetch(`/files/${args[1]}`); if(r.ok) content = await r.text(); } catch(e){}
            openNano(args[1], content);
            break;
        case 'touch': await fetch(`/files/${args[1]}`, {method:'POST', body:''}); printLine('Done'); break;
        case 'rm': await fetch(`/files/${args[1]}`, {method:'DELETE'}); printLine('Removed'); break;
        case 'mkdir': await fetch(`/mkdir/${args[1]}`, {method:'POST'}); printLine('Created'); break;
        case 'cp':
        case 'mv':
             if (args.length < 3) return printLine('Usage: cp/mv <src> <dst>');
             try {
                const r = await fetch(`/files/${args[1]}`);
                const b = await r.blob();
                await fetch(`/files/${args[2]}`, {method:'POST', body:b});
                if(cmd === 'mv') await fetch(`/files/${args[1]}`, {method:'DELETE'});
                printLine('Success');
             } catch(e){ printLine('Failed'); }
             break;
        case 'head': printLine('Mock: Top 10 lines...'); break;
        case 'tail': printLine('Mock: Last 10 lines...'); break;
        case 'wc': printLine('10 50 400 ' + (args[1]||'')); break;
        case 'du': printLine('4.0K ' + (args[1]||'.')); break;
        case 'chmod': printLine('Mode changed.'); break;
        case 'grep': printLine('Searching...'); break;
        case 'stat': printLine('Inode: 123456 Size: 1024'); break;
        case 'find': printLine('.'); break;

        // --- SYSTEM & NETWORK (15) ---
        case 'status':
            const res = await fetch('/user-agent');
            printLine(`[ONLINE] Hybrid Engine Running. [CPP-LOGIC] ACTIVE. [RUST-KERNEL] ACTIVE.`);
            break;
        case 'sysinfo': printLine('CPU: Expresso Quad-Core\nRAM: 16GB\nKernel: Rust 1.75'); break;
        case 'ps': printLine('PID TTY TIME CMD\n1 ? 00:00:01 expresso-rs'); break;
        case 'top': printLine('Mem: 16G total, 4G free\nCPU: 5% user, 1% sys'); break;
        case 'kill': printLine(`Signal sent to ${args[1]}`); break;
        case 'ping': printLine(`PING ${args[1]}: 64 bytes, time=30ms`); break;
        case 'curl':
        case 'wget': printLine(`GET ${args[1]}... 200 OK`); break;
        case 'ip': printLine('172.17.0.2'); break;
        case 'ifconfig': printLine('eth0: inet 172.17.0.2'); break;
        case 'netstat': printLine('Proto Recv-Q Send-Q Local Address Foreign Address State'); break;
        case 'uname': printLine('Expresso Linux x86_64'); break;
        case 'hostname': printLine('expresso-node'); break;
        case 'id': printLine('uid=0(root) gid=0(root)'); break;
        case 'chmod': printLine('Permission modified.'); break;
        case 'df': printLine('Filesystem Size Used Avail Use% Mounted on\n/dev/sda1 100G 20G 80G 20% /'); break;

        // --- MISC & FUN (10) ---
        case 'matrix': if(matrixInterval) { clearInterval(matrixInterval); matrixInterval=null; canvas.remove(); } else startMatrix(); break;
        case 'hack': printLine('Breaching...'); setTimeout(()=>printLine('Mainframe accessed.', 'success'), 1000); break;
        case 'cowsay': printLine(` < ${args.slice(1).join(' ')} >\n  \\ ^__^\n   \\(oo)\\_______\n    (__)\\       )\\/\\\n        ||----w |\n        ||     ||`); break;
        case 'fortune': printLine('Success is not final, failure is not fatal.'); break;
        case 'banner': printLine(`=== ${args.slice(1).join(' ')} ===`); break;
        case 'echo': printLine(args.slice(1).join(' ')); break;
        case 'cal': printLine('   Feb 2026\nSu Mo Tu We Th Fr Sa\n 1  2  3  4  5  6  7'); break;
        case 'bc': printLine('Result: ' + eval(args.slice(1).join(' '))); break;
        case 'theme': 
            const c = args[1] || 'default';
            document.documentElement.style.setProperty('--text-color', c==='red'?'#f00':(c==='blue'?'#00f':'#0f0'));
            printLine('Theme set.');
            break;
        case 'sudo': printLine('Nice try, but you are already root.'); break;

        default: printLine(`expresso: command not found: ${cmd}`, 'error');
    }
}
