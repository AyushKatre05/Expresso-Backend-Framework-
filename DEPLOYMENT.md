# Deploying Expresso to Render

## Prerequisites
- A GitHub/GitLab repository with this code pushed.
- A [Render.com](https://render.com) account.

## Option 1: Blueprint (Recommended)
1.  Log in to the Render Dashboard.
2.  Click **New +** -> **Blueprints**.
3.  Connect your repository.
4.  Render will automatically detect the `render.yaml` file.
5.  Click **Apply**.
6.  Your server will be deployed!

## Option 2: Manual Web Service
1.  Click **New +** -> **Web Service**.
2.  Connect your repository.
3.  **Runtime:** Docker
4.  **Region:** Singapore (or nearest)
5.  **Environment Variables**:
    -   `PORT`: `10000` (or any port, Render assigns one automatically)

## Testing Deployed App
Once deployed, Render will give you a URL (e.g., `https://expresso-framework.onrender.com`).
-   Visit the URL to see your **Expresso Dashboard**.
-   Use the "Echo Tester" to verify the server is running.
