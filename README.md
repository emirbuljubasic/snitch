<p align="center">
  <img src="logo.png" alt="Logo" width="200" />
</p>

<h1 align="center">Snitch</h1>

<p align="center">
  A stealthy monitor for <code>/dev/pts</code> activity.<br />
  Watch live SSH sessions. Know what’s happening. In real-time.
</p>

<p align="center">
  <a href="#installation">Installation</a> •
  <a href="#usage">Usage</a> •
  <a href="#features">Features</a> •
  <a href="#contributing">Contributing</a> •
  <a href="#license">License</a>
</p>

---

## 🚀 Overview

**snitch** is a lightweight C tool for silently monitoring activity on `/dev/pts`, allowing you to observe real-time SSH sessions and terminal input across your system.  
Built for Linux environments, it’s a powerful tool for admins, auditors, or anyone curious about what's happening on their machines.

---

## 🛠️ Installation

### Prerequisites

- `make`
- A POSIX-compatible shell
- A Linux system with access to `/dev/pts`

### From source

```bash
git clone https://github.com/yourusername/snitch.git
cd snitch
make
```

---

## 📦 Usage

Basic usage:

```bash
./snitch
```

Optional flags (if available):

```bash
./snitch -v   # verbose mode
```

---

## ✨ Features

- ✅ Monitors live `/dev/pts` terminals
- 🔍 See what users are doing in real-time
- 👻 Stealthy and lightweight – runs in the background
- 🔐 Useful for auditing, forensics, and investigation

---

## 📚 Documentation

More details will be available in the [`/docs`](./docs) directory (coming soon).  
Until then, see usage examples above or run `./snitch --help` (if implemented).

---

## 🧩 Configuration

No configuration is required by default.  
Advanced users may tweak source code or compile-time options to fit their use case.

---

## 🧪 Testing

```bash
make test
```

*Note: Add test scripts or manual test steps as needed.*

---

## 🤝 Contributing

Contributions are welcome! Please open an issue or submit a pull request.

See [`CONTRIBUTING.md`](./CONTRIBUTING.md) for guidelines.

---

## 📄 License

This project is licensed under the [MIT License](./LICENSE).

---

<p align="center">Made with ❤️ by <a href="https://github.com/emirbuljubasic">emirbuljubasic</a></p>
