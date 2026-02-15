const output = document.getElementById('output');
const input = document.getElementById('cmdInput');
const promptStr = 'root@expresso:~$ ';

input.addEventListener('keydown', async (e) => {
    if (e.key === 'Enter') {
        const cmd = input.value.trim();
        if (cmd) {
            printLine(promptStr + cmd, 'command-echo');
            await processCommand(cmd);
        }
        input.value = '';
        window.scrollTo(0, document.body.scrollHeight);
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
        case 'help':
            printLine(`Available commands:
  status        Show connection status & hybrid engine details
  ls            List files in the directory
  cat <file>    Read content of a file
  post <file> <content>  Create a file with content
  get <path>    Perform RAW GET request (e.g., get /user-agent)
  echo <msg>    Test echo API
  clear         Clear terminal`, 'info');
            break;

        case 'clear':
            output.innerHTML = '';
            break;

        case 'status':
            try {
                const res = await fetch('/user-agent');
                const ua = await res.text();
                printLine(`[STATUS] Online`, 'success');
                printLine(`[ENGINE] Hybrid Rust/C/C++ Architecture`, 'info');
                printLine(`[USER-AGENT] ${ua}`);
            } catch (e) {
                printLine(`[ERROR] Connection failed`, 'error');
            }
            break;

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

        case 'post':
            if (args.length < 3) return printLine('Usage: post <filename> <content...>', 'error');
            const filename = args[1];
            const content = args.slice(2).join(' ');
            try {
                const res = await fetch(`/files/${filename}`, {
                    method: 'POST',
                    body: content
                });
                if (res.ok) printLine(`File '${filename}' created successfully.`, 'success');
                else printLine(`Error: ${res.status}`, 'error');
            } catch(e) { printLine('Network Error', 'error'); }
            break;

        case 'get':
            if (!args[1]) return printLine('Usage: get <path>', 'error');
            try {
                const res = await fetch(args[1]);
                const text = await res.text();
                printLine(`[${res.status}] ${text}`);
            } catch(e) { printLine('Network Error', 'error'); }
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
