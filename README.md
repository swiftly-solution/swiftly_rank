<p align="center">
  <a href="https://github.com/swiftly-solution/swiftly_rank">
    <img src="https://cdn.swiftlycs2.net/swiftly-logo.png" alt="SwiftlyLogo" width="80" height="80">
  </a>

  <h3 align="center">[Swiftly] Rank System</h3>

  <p align="center">
    A simple plugin for Swiftly that implements an Rank System.
    <br/>
  </p>
</p>

<p align="center">
  <img src="https://img.shields.io/github/downloads/swiftly-solution/swiftly_rank/total" alt="Downloads"> 
  <img src="https://img.shields.io/github/contributors/swiftly-solution/swiftly_rank?color=dark-green" alt="Contributors">
  <img src="https://img.shields.io/github/issues/swiftly-solution/swiftly_rank" alt="Issues">
  <img src="https://img.shields.io/github/license/swiftly-solution/swiftly_rank" alt="License">
</p>

---

### Installation 👀

1. Download the newest [release](https://github.com/swiftly-solution/swiftly_rank/releases).
2. Everything is drag & drop, so i think you can do it!
3. Setup database connection in `addons/swiftly/configs/databases.json` with the key `swiftly_ranks` like in the following example:
```json
{
    "swiftly_ranks": {
        "host": "...",
        "username": "...",
        "password": "...",
        "database": "...",
        "port": 3306
    }
}
```
(!) Don't forget to replace the `...` with the actual values !!

### Configuring the plugin 🧐

* After installing the plugin, you should change the default prefix from ``addons/swiftly/translations/translation.swiftly_ranks.json`` (optional)
* To change the value of the points, edit ``addons/swiftly/configs/plugins/swiftly_ranks.json``
* If you want to enable Clan Tags with the ranks in scoreboard, you will need to edit the config placed in: ``addons/swiftly/configs/plugins/swiftly_ranks.json``

### TODO 🛡️

* Top Command.
* Rank Icons in Scoreboard.
* Chat Alert when reaching a new rank.

### Creating A Pull Request 😃

1. Fork the Project
2. Create your Feature Branch
3. Commit your Changes
4. Push to the Branch
5. Open a Pull Request

### Have ideas/Found bugs? 💡
Join [Swiftly Discord Server](https://swiftlycs2.net/discord) and send a message in the topic from `📕╎plugins-sharing` of this plugin!

---