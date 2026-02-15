document.addEventListener('DOMContentLoaded', () => {
    // 1. Fetch User Agent to prove server interaction
    fetch('/user-agent')
        .then(res => res.text())
        .then(text => {
            const el = document.getElementById('userAgent');
            // Clean up the header string if it contains "User-Agent: "
            el.textContent = text.replace('User-Agent:', '').trim();
        })
        .catch(err => {
            document.getElementById('userAgent').textContent = 'Error connecting';
            document.getElementById('serverStatus').innerHTML = '<span class="dot" style="background:red;box-shadow:0 0 8px red"></span> Offline';
            document.getElementById('serverStatus').style.color = '#ef4444';
            document.getElementById('serverStatus').style.background = 'rgba(239, 68, 68, 0.1)';
        });
});

// 2. Test Echo Endpoint
function testEcho() {
    const input = document.getElementById('echoInput').value;
    const output = document.getElementById('echoOutput');
    
    if (!input) return;

    output.innerHTML = '<span class="placeholder">Sending...</span>';

    fetch(`/echo/${encodeURIComponent(input)}`)
        .then(res => {
            if (!res.ok) throw new Error('Network response was not ok');
            return res.text();
        })
        .then(text => {
            output.textContent = text;
        })
        .catch(err => {
            output.innerHTML = '<span style="color: #ef4444">Error: Could not reach server.</span>';
        });
}

// 3. File Check (Mock/Real)
function checkFile(filename) {
    const status = document.getElementById('fileStatus');
    status.textContent = 'Checking...';
    
    fetch(`/files/${filename}`)
        .then(res => {
            if (res.status === 200) {
                status.innerHTML = `<span style="color:var(--success)">Found ${filename}! (200 OK)</span>`;
            } else if (res.status === 404) {
                status.innerHTML = `<span style="color:#f59e0b">File not found (404). Try creating it via POST.</span>`;
            } else {
                status.textContent = `Status: ${res.status}`;
            }
        })
        .catch(err => {
            status.innerHTML = '<span style="color: #ef4444">Connection Error</span>';
        });
}
