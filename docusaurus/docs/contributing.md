---
title: Contributing
sidebar_position: 6
---

# 🤝 Contributing

We welcome contributions to the Lightweight Secure TCP project!

Whether it's a bug fix, new feature, test improvement, or documentation update, your help is appreciated.

---

## 🧰 How to Contribute

1. **Fork the Repository**

   Go to [GitHub](https://github.com/sDenizOzturk/lightweight-secure-tcp) and click the **Fork** button.

2. **Clone Your Fork**

   ```bash
   git clone https://github.com/YOUR_USERNAME/lightweight-secure-tcp.git
   cd lightweight-secure-tcp
   ```

3. **Create a New Branch**

   ```bash
   git checkout -b your-meaningful-branch-name
   ```

4. **Make Your Changes**

   - For **core C++ code**, edit files under `lib/LightweightSecureTCP/`.
   - For **examples**, use the `examples/` directory.
   - For **documentation**, update markdown files inside `docusaurus/docs/`.

5. **Build and Test**

   Use the `test_runner` executable to verify that your changes don’t break existing logic.

   - The project uses a top-level `CMakeLists.txt`. You can open this file directly in your IDE.
   - If you're using **Qt Creator**, simply open the root `CMakeLists.txt`, select a kit, and build `test_runner`.

   You can also add your own test cases inside the `tests/` directory.

6. **Build the Documentation (if edited)**

   ```bash
   cd docusaurus
   npm install
   npm start
   npm run build
   ```

   Make sure the documentation builds successfully before committing.

7. **Commit and Push**

   ```bash
   git add .
   git commit -m "your-clear-and-descriptive-message"
   git push origin your-meaningful-branch-name
   ```

8. **Open a Pull Request**

   Go to your fork and open a PR against the `main` branch of the original repo.

---

## 💡 Tips

- Keep commits focused and atomic.
- Use descriptive PR titles and explanations.
- Test your code before submitting.

Thanks for contributing!
