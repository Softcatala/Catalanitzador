# **Microsoft Windows**

## Steps to follow when doing a release

- Build all the software (`Build All`) to update the macros `_DATE_` and `_TIME_`.
- Sign the binary digitally:
  ```bash
  signtool sign /t http://timestamp.digicert.com /f ../certificat.pfx /p PASSWORD CatalanitzadorPerAlWindows.exe
  ```
- Run all the unit tests (in **release** mode).
- Run the `DownloadFilesChecker` to verify that all server files are correct.
- Upload `configuration.xml` and the corresponding `.sha1` signature file to:
  ```
  ftp://ftp.softcatala.org/pub/softcatala/catalanitzador/configuration.xml
  ```
- Upload the binary and corresponding `.sha1` signature file to:
  ```
  ftp://ftp.softcatala.org/pub/softcatala/catalanitzador/
  ```
- Make sure that the versions and links are correct in the download page:
  ```
  https://www.softcatala.org/catalanitzador/
  ```
- Notify the new version to the Telegram channel.

- Tag the release in Git:
  ```bash
  git tag -m "X.Y.X tag" -a X_Y_X
  git push origin X_Y_X
  ```
  Where `X_Y_X` is the version published.

---

## **Checklist**

- Download `CatalanitzadorPerAlWindows.exe` from its final location and execute it.
- In the `CatalanitzadorPerAlWindows.log`, ensure the following line appears:
  ```
  ConfigurationDownloadThread::OnStart. Remote file same version that the embedded
  ```
  This indicates the embedded file and the remote `configuration.xml` are identical.
- Execute the previous version and verify that it auto-updates.

---

## **Changing `configuration.xml`**

- Update the `<date>` field with the date of the modifications.
  > Catalanitzador only uses the remote file if its date is newer than the embedded version.
  
- Run `DownloadFilesChecker` to verify correctness.

- If you've made breaking changes to the XML and all users use this URL, update the `<version>` tag in the compatibility section to the most recent version to prevent older clients from using the file.

---

# **Mac OS X**

- Open Xcode and go to: `Product → Archive → Distribute App`
- When prompted to "Select method of distribution", choose:
  - `Custom → Developer ID`
  - Then select `Upload` (to notary service).
- Create a zip file of the output:
  ```bash
  zip -r CatalanitzadorPerAlMac.zip CatalanitzadorPerAlMac.app/
  ```
