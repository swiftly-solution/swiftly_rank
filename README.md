<br/>
<p align="center">
  <a href="https://github.com/swiftly-solution/swiftly_rank">
    <img src="https://media.discordapp.net/attachments/979452783466000466/1168236894652469248/Swiftly_Logo.png?ex=6575f264&is=65637d64&hm=dd2834983bebeab98d7febd44bb3bd20e9aded13ecefac63cc990b222a9d9e9e&=&format=webp&quality=lossless&width=468&height=468" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">[Swiftly] Rank System</h3>

  <p align="center">
    A simple plugin for Swiftly that implements an Rank System.
    <br/>
    <br/>
    <a href="https://github.com/swiftly-solution/swiftly_rank/issues">Report Bug</a>
    .
    <a href="https://github.com/swiftly-solution/swiftly_rank/issues">Request Feature</a>
  </p>
</p>


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

### Have ideas? 💡
Join [Swiftly Discord Server](https://swiftlycs2.net/discord)!