Hier ist ein **einfaches, neutrales Styleguide-Template in Markdown**, komplett leer zum Ausfüllen:

---

# Rust Styleguide (Template)

## 1. Allgemeines

* Ziel des Styleguides:
* Gültig für folgende Projekte/Teams:

---

## 2. Codeformatierung

* Einrückung:
1 Tab => 4 Spaces
```
foo()
{
    return do();
}
```
* Zeilenlänge: 120
* Klammerstil:
```
foo()
{

}
```
* Leerzeilen-Regeln:
- Zeilen in Funktion: 1  
- Klassen: 2

---

## 3. Namenskonventionen

### Variablen:
```
lower_snake_case
```

### Funktionen:
```
lower_snake_case
```
### Structs & Enums:
```
UpperCammelCase
```
### Konstanten:
```
UPPER_SNAKE_CASE
```
### Module & Dateien:
- lower_snake_case
---

## 4. Projektstruktur


---

## 5. Funktionen und Methoden

* Parameteranzahl: Beliebig.
* Fehlerbehandlung innerhalb von Funktionen:
- Parameter müssen Geprüft werden.
* Regeln für Helper-Funktionen:
- Klein halten nur eine Aufgabe pro Helper.

---

## 7. Fehlerbehandlung

* Eigenen Error-Typ definieren:
- ja und Fehler Code Strucktur erarbeiten.

---

## 9. Dokumentation

* doc-Kommentare (`///`):
* Modulkommentare (`//!`):
* Beispielcode:
```
///foo
///methode die do() ausführt.
///
///
///
foo()
{
    do() // do wird ausgeführt und rückgabe wert zurückgegeben.
}
```

---

## 10. Tests

* Unit-Tests:
* Integration-Tests:
* Mocking:

---


## 12. Performance

* Datenstrukturen:
* Iteratoren:
* `unsafe`-Regeln:

---

## 13. Sicherheit

* Input-Validierung:
* Logging sensibler Daten:

---

