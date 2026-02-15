const output = document.getElementById('output');
const input = document.getElementById('cmdInput');
const promptStr = 'root@expresso:~$ ';
let commandHistory = [];
let historyIndex = -1;

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

    const katakana = 'アァカサタナハマヤャラワガザダバパイィキシチニヒミリヰギジヂビピウゥクスツヌフムユュルグズブヅプエェケセテネヘメレヱゲゼデベペオォコソトノホモヨョロヲゴゾドボポ1234567890';
    const nums = '01';
    const alphabet = katakana + nums;

    const fontSize = 16;
    const columns = canvas.width / fontSize;
    const drops = [];

    for( let x = 0; x < columns; x++ ) {
        drops[x] = 1;
    }

    const draw = () => {
        ctx.fillStyle = 'rgba(0, 0, 0, 0.05)';
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        
        ctx.fillStyle = '#0F0'; // Green text
        ctx.font = fontSize + 'px monospace';

        for(let i = 0; i < drops.length; i++) {
            const text = alphabet.charAt(Math.floor(Math.random() * alphabet.length));
            ctx.fillText(text, i*fontSize, drops[i]*fontSize);
            
            if(drops[i]*fontSize > canvas.height && Math.random() > 0.975)
                drops[i] = 0;
            
            drops[i]++;
        }
    };
    matrixInterval = setInterval(draw, 30);
    printLine('Matrix Protocol Initiated...', 'success');
}

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

// Focus input on click anywhere
document.addEventListener('click', () => input.focus());

function printLine(text, className = '') {
    const div = document.createElement('div');
    div.className = className || 'command-output';
    div.textContent = text;
    output.appendChild(div);
}

async function processCommand(cmdLine) {
    const args = cmdLine.split(' ');
    const cmd = args[0].toLowerCase();

    switch (cmd) {
        // --- CORE ---
        case 'help':
            printLine(`
  --- CORE COMMANDS ---
  help         : Show this help menu
  clear        : Clear terminal screen
  history      : Show command history
  theme <color>: Change theme (green, amber, blue, red)
  matrix       : Toggle Matrix rain effect
  
  --- SYSTEM ---
  status       : Show Hybrid Engine status (Rust/C/C++)
  whoami       : Show current user
  uptime       : Show server uptime (mock)
  sysinfo      : Show simulated hardware stats
  date         : Show current server date/time
  
  --- FILE OPERATIONS ---
  ls           : List files in directory
  cat <file>   : Read file content
  rm <file>    : Delete file or directory
  mkdir <dir>  : Create a directory
  touch <file> : Create an empty file
  cp <src> <dst>: Copy a file (client-side simulation)
  mv <src> <dst>: Move/Rename a file (client-side simulation)
  
  --- NETWORK ---
  ping <host>  : Simulate ping
  curl <url>   : Simulate curl request
  ip           : Show client IP (mock)
  headers      : Show request headers
  
  --- FUN ---
  hack         : Simulate hacking sequence
  echo <msg>   : Echo message
            `, 'info');
            break;

        case 'clear':
            output.innerHTML = '';
            break;
            
        case 'history':
            printLine(commandHistory.map((c, i) => `${i+1}  ${c}`).join('\n'));
            break;

        case 'theme':
            const color = args[1];
            const root = document.documentElement;
            if (color === 'amber') {
                root.style.setProperty('--text-color', '#ffb000');
                root.style.setProperty('--prompt-color', '#ffb000');
            } else if (color === 'blue') {
                root.style.setProperty('--text-color', '#00bfff');
                root.style.setProperty('--prompt-color', '#00bfff');
            } else if (color === 'red') {
                root.style.setProperty('--text-color', '#ff5555');
                root.style.setProperty('--prompt-color', '#ff5555');
            } else {
                 root.style.setProperty('--text-color', '#00ff41');
                 root.style.setProperty('--prompt-color', '#bd93f9');
            }
            printLine(`Theme changed to ${color || 'default'}.`);
            break;

        case 'matrix':
            if (matrixInterval) {
                clearInterval(matrixInterval);
                matrixInterval = null;
                if(canvas.parentNode) canvas.parentNode.removeChild(canvas);
                printLine('Matrix Protocol Terminated.');
            } else {
                startMatrix();
            }
            break;

        // --- SYSTEM ---
        case 'status':
            try {
                const res = await fetch('/user-agent');
                const ua = await res.text();
                printLine(`[STATUS] System Online`, 'success');
                printLine(`[ENGINE] Hybrid Rust/C/C++ Architecture Active`, 'success');
                printLine(`[KERNEL] Expresso 1.0.0 (Rust Native)`);
                printLine(`[PARSER] C Legacy Parser (FFI Linked) - ACTIVE`, 'info');
                printLine(`[LOGIC]  C++ Request Handler (FFI Linked) - ACTIVE`, 'info');
                printLine(`[CLIENT] ${ua}`);
            } catch (e) { printLine(`[ERROR] Connection failed`, 'error'); }
            break;

        case 'whoami':
            printLine('root');
            break;
            
        case 'uptime':
             printLine(`up ${Math.floor(performance.now() / 60000)} mins, 1 user, load average: 0.00, 0.01, 0.05`);
             break;
             
        case 'sysinfo':
             printLine(`CPU: Expresso Virtual Core @ 3.5GHz
RAM: 16GB / 32GB
OS:  Rust/Linux Hybrid
GPU: WebGL Terminal Adapter`);
             break;
             
        case 'date':
             printLine(new Date().toString());
             break;

        // --- FILES ---
        case 'ls':
            try {
                const res = await fetch('/files');
                if (res.ok) {
                    const text = await res.text();
                    printLine(text || '(empty directory)');
                } else printLine(`Error: ${res.status}`, 'error');
            } catch(e) { printLine('Network Error', 'error'); }
            break;

        case 'cat':
            if (!args[1]) return printLine('Usage: cat <filename>', 'error');
            try {
                const res = await fetch(`/files/${args[1]}`);
                if (res.ok) {
                    const text = await res.text();
                    printLine(text);
                } else printLine(`File not found: ${args[1]}`, 'error');
            } catch(e) { printLine('Network Error', 'error'); }
            break;
            
        case 'rm':
            if (!args[1]) return printLine('Usage: rm <filename>', 'error');
            try {
                const res = await fetch(`/files/${args[1]}`, { method: 'DELETE' });
                if (res.ok) printLine(`Deleted ${args[1]}`, 'success');
                else printLine(`Error deleting ${args[1]}: ${res.statusText}`, 'error');
            } catch(e) { printLine('Network Error', 'error'); }
            break;
            
        case 'mkdir':
             if (!args[1]) return printLine('Usage: mkdir <dirname>', 'error');
             try {
                // We use a POST request to /mkdir/<name> for convenience
                const res = await fetch(`/mkdir/${args[1]}`, { method: 'POST' });
                if (res.ok) printLine(`Directory ${args[1]} created.`, 'success');
                else printLine(`Error: ${res.status}`, 'error');
            } catch(e) { printLine('Network Error', 'error'); }
            break;
            
        case 'touch':
            if (!args[1]) return printLine('Usage: touch <filename>', 'error');
            try {
                const res = await fetch(`/files/${args[1]}`, {
                    method: 'POST',
                    body: ''
                });
                if (res.ok) printLine(`Touched ${args[1]}`, 'success');
                else printLine(`Error: ${res.status}`, 'error');
            } catch(e) { printLine('Network Error', 'error'); }
            break;
            
        case 'cp':
             if (args.length < 3) return printLine('Usage: cp <src> <dst>', 'error');
             try {
                 // Read src
                 const getRes = await fetch(`/files/${args[1]}`);
                 if (!getRes.ok) throw new Error('Source file not found');
                 const content = await getRes.blob();
                 
                 // Write dst
                 const postRes = await fetch(`/files/${args[2]}`, { method: 'POST', body: content });
                 if (postRes.ok) printLine(`Copied ${args[1]} -> ${args[2]}`, 'success');
                 else printLine('Failed to create destination file', 'error');
             } catch(e) { printLine(e.message, 'error'); }
             break;

        case 'mv':
             // Copy then Delete
             if (args.length < 3) return printLine('Usage: mv <src> <dst>', 'error');
             try {
                 // 1. Copy (CP logic)
                 const getRes = await fetch(`/files/${args[1]}`);
                 if (!getRes.ok) throw new Error('Source file not found');
                 const content = await getRes.blob();
                 
                 const postRes = await fetch(`/files/${args[2]}`, { method: 'POST', body: content });
                 if (!postRes.ok) throw new Error('Failed to create destination');
                 
                 // 2. Delete (RM logic)
                 const delRes = await fetch(`/files/${args[1]}`, { method: 'DELETE' });
                 if (delRes.ok) printLine(`Moved ${args[1]} -> ${args[2]}`, 'success');
                 else printLine('Warning: Copied but failed to delete source', 'info');
                 
             } catch(e) { printLine(e.message, 'error'); }
             break;

        // --- NETWORK ---
        case 'ping':
             if (!args[1]) return printLine('Usage: ping <host>', 'error');
             printLine(`Pinging ${args[1]}...`);
             setTimeout(() => printLine(`64 bytes from ${args[1]}: icmp_seq=1 ttl=64 time=0.045 ms`, 'success'), 500);
             setTimeout(() => printLine(`64 bytes from ${args[1]}: icmp_seq=2 ttl=64 time=0.052 ms`, 'success'), 1000);
             setTimeout(() => printLine(`64 bytes from ${args[1]}: icmp_seq=3 ttl=64 time=0.048 ms`, 'success'), 1500);
             break;
             
        case 'curl':
             if (!args[1]) return printLine('Usage: curl <url>', 'error');
             printLine(`> GET ${args[1]} HTTP/1.1\n> User-Agent: curl/7.68.0\n< HTTP/1.1 200 OK\n... (simulated response)`, 'info');
             break;
             
        case 'ip':
             printLine('192.168.1.105 (Local) / 10.0.0.5 (Container)');
             break;
             
        case 'headers':
             printLine('User-Agent: Expresso-Terminal/1.0\nAccept: */*\nConnection: keep-alive');
             break;

        // --- FUN ---
        case 'hack':
             printLine('Initiating brute force...', 'error');
             let count = 0;
             const hackInt = setInterval(() => {
                 printLine(`Accessing port ${Math.floor(Math.random() * 65535)}... [OPEN]`, 'success');
                 count++;
                 if (count > 5) {
                     clearInterval(hackInt);
                     printLine('ACCESS GRANTED. Mainframe breached.', 'success');
                 }
             }, 300);
             break;

        case 'echo':
            const msg = args.slice(1).join(' ');
            try {
                const res = await fetch(`/echo/${encodeURIComponent(msg)}`);
                const text = await res.text();
                printLine(text);
            } catch(e) { printLine('Network Error', 'error'); }
            break;

        default:
            printLine(`Command not found: ${cmd}. Type 'help'.`, 'error');
    }
}
