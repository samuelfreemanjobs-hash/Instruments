# Bump version

1. Read `project(... VERSION x.y.z)` in the CMakeLists the user named. Do not bump every product.
2. Set the new version in that `project()` call and add a dated entry to that product's `CHANGELOG.md`.
3. Show the diff. Do not `git tag` or `git push` unless the user says go.
4. If they say go, tag `vX.Y.Z` on the release commit and stop before any force-push.
