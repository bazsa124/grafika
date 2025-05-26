# 🧟‍♂️ 3D Zombis Lövöldözős Játék – Grafika beadandó (2025)

### Készítette: **Csákó Balázs**  
### Modellek: **Kiss Konrád Soma, Csákó Balázs**

Ez a projekt egy egyetemi grafika beadandó keretében készült. A cél egy 3D-s túlélő játék létrehozása volt, ahol a játékos egy végtelen zombihullámokkal teli pályán próbál életben maradni. A harc kizárólag lőfegyverrel történik.

## 🕹️ Játékmenet összefoglaló

A játékos egy zárt pályán mozog, zombik követik. A cél a túlélés és az ellenségek legyőzése. A játék akkor ér véget, ha a játékos életereje 0-ra csökken vagy ha minden zombi meghalt.

A játék végi üzenet:  
**„You died.”/„Victory”**

## ⚙️ Alapfunkciók

- **Kamera:** Egérmozgással irányítható
- **Mozgás:** WASD billentyűkkel
- **Objektumkezelés:** Minden játékelem `Thing` struktúrában tárolva, láncolt listában
- **Animáció:** Procedurálisan generált mozgás szinuszhullámok segítségével
- **Textúrázás:** `stb_image` könyvtárral történik
- **Fények:** Dinamikus fényforrás, amely követi a kamerát és szabályozható

## ✨ Extra funkciók

- **Zombi AI:**
  - Zombik automatikusan a játékos felé mozognak
  - Ütközéskor animáció, sebzést okoznak
  - Sebződés után animációt játszanak le, halál esetén eltűnnek

- **Lőfegyverhasználat:**
  - A játékos csak lőfegyvert használhat
  - Lőszerkészlet a képernyő **jobb alsó sarkában** látható
  - Lövés és újratöltés animációk

- **Ellenségek megjelenése:**
  - Egyszer 20, a játékos körül kör véletlen helyen

- **Felhasználói felület:**
  - **Életerő sáv** - lent középen
  - **Lőszer számláló** – jobb alsó sarok
  - **Help menü és exit** – jobb felső sarok
  - **Játék végi statisztika:** Meghaltál / Nyertél
  - **Környezeti változók:** Help-ben jélzett gomb kombinációkkal

- **Köd:** A játéktérben állítható sűrűségű köd biztosít atmoszférát

## 📜 Licensz

A projekt az [MIT licenc](https://choosealicense.com/licenses/mit/) alatt érhető el.

### Felhasznált könyvtárak:

- [OpenGL](https://www.opengl.org/about/#11) – [Khronos License](https://www.khronos.org/legal/)
- [SDL2](https://www.libsdl.org/license.php) – [zLib License](https://opensource.org/licenses/Zlib)
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/) – [zLib License](https://opensource.org/licenses/Zlib)
- [stb_image](https://github.com/nothings/stb) – [MIT License](https://opensource.org/licenses/MIT)
