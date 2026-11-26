# Findings

Pre-existing issues and follow-ups found while working on other tasks, kept out
of those changes. See "Scope Discipline" in `AGENTS.md`; the severity uses the
P0-P5 scale of "Debug And Ticket Priorities" there.

## The MGM logs every refused PIO open of an obfuscated or encrypted file at ERROR

- File: `mgm/ofs/XrdMgmOfsFile.cc:1733-1745` (PIO refusal in
  `XrdMgmOfsFile::open`), `mgm/ofs/XrdMgmOfsFile.cc:4417` (`eos_err` in
  `XrdMgmOfsFile::Emsg`).
- Symptom: every PIO open of an obfuscated or encrypted file, read or write,
  is refused with `ENOTSUP` and logged at ERROR through `Emsg`. The refusal is
  expected: the RAIN plugin always falls back to a gateway access, and the
  access works. A client using the plugin on such files produces one ERROR line
  per file in the MGM log.
- Evidence: `Emsg` calls `eos_err` for every error it returns. VM check of
  2026-10-23 (EOS-6676): `Unable to open - PIO is not supported for obfuscated
  or encrypted files /eos/dev/test/piofb/f; Operation not supported`, followed
  by a successful gateway write.
- Severity: P4 (wrong log: an expected refusal the client recovers from is
  logged at ERROR; no data or metadata damage).
- Blocks the task: no (follow-up of MR !522, EOS-6676).

## The RAIN plugin hides why the MGM refused a PIO open

- File: `fst/xrdcl_plugins/RainFile.cc:248` (`OpenPio`), reported at
  `fst/xrdcl_plugins/RainFile.cc:162` and `:183`.
- Symptom: when the MGM refuses the PIO open request, `OpenPio` returns its own
  `errNotImplemented` status ("error PIO open request") instead of the status
  of the query. The fallback warnings, and the caller, then only show
  `[ERROR] Operation is not implemented`, whatever the reason: obfuscated or
  encrypted file, non-RAIN layout, anonymous client.
- Evidence: plugin log of the VM check of 2026-10-23 (EOS-6676):
  `failed PIO write open, fall back to gateway write`,
  `err="[ERROR] Operation is not implemented"`; the reason was only in the MGM
  log. `RainFile.cc:246` logs the status of the query at ERROR before replacing
  it; whether that status carries the MGM's message was not checked.
- Severity: P4 (wrong error output: the plugin reports the wrong reason; the
  fallback works and nothing persists).
- Blocks the task: no (follow-up of MR !522, EOS-6676).

## `convert rule` is advertised by the console but refused by the MGM

- File: `mgm/proc/admin/ConvertCmd.cc:67-85` (`ConvertCmd::ProcessRequest`),
  `console/commands/native/convert-proto-native.cc:19`, `:31-32`, `:92-101`,
  `:247`.
- Symptom: the console help lists `convert rule <identifier> <conversion>`
  ("apply a conversion rule on the given directory") and the console builds a
  `ConvertProto_RuleProto` request, but the MGM only dispatches `kConfig`,
  `kFile`, `kList` and `kClear`. A rule request falls through to
  `error: command not supported` with `EINVAL`.
- Evidence: code reading only (2026-11-26, while answering a space migration
  question). There is no `kRule` branch in `ConvertCmd.cc`; not reproduced on
  a running MGM (no server build available in that session).
- Severity: P4 (wrong behavior: an advertised admin command does not work,
  wrong error code; nothing is changed).
- Blocks the task: no.

## `fs mv <fsid> <space>.<index>` lets a group grow one past `groupsize`

- File: `mgm/proc/proc_fs.cc:1000` (`proc_mv_fs_group`), compared with
  `mgm/proc/proc_fs.cc:1211` (`proc_sort_groups_by_priority`).
- Symptom: moving a file system into an explicitly named group is refused only
  when the group already has more than `groupsize` file systems
  (`grp_num_fs > grp_size`). A group with exactly `groupsize` entries still
  accepts one more, so it ends up with `groupsize + 1`. The automatic group
  selection used by `fs mv <fsid> <space>` / `fs add` keeps only groups with
  `size() < grp_size`, so the two paths disagree on the limit.
- Evidence: code reading only (2026-11-26); not reproduced on a running MGM.
- Severity: P4 (wrong admin-side check; the extra file system is placed
  normally and can be moved back, no data or metadata damage).
- Blocks the task: no.

## `node config` with any `*` in the node name applies to every node

- File: `mgm/proc/admin/NodeCmd.cc:345-349` (`NodeCmd::ConfigSubcmd`), applied
  by `NodeCmd::ConfigFsSpecific` (`mgm/proc/admin/NodeCmd.cc:465-530`).
- Symptom: if the node name contains `*` anywhere, the MGM selects all entries
  of `mNodeView` instead of matching a pattern. A command meant for one box,
  for example `eos node config fst05* configstatus=drain`, sets every file
  system of the instance to drain (read-only for placement), so no new file
  can be written until the status is set back. The same applies to `sched=`.
  The console help only documents a substring match for `node ls`.
- Evidence: code reading only (2026-11-26); not reproduced on a running MGM.
- Severity: P4 (wrong and surprising admin behavior, recoverable by resetting
  the status; no data or metadata damage. Instance-wide write refusal is not in
  the P3 list of crash/deadlock/hang/resource exhaustion, but it is close to it.)
- Blocks the task: no.
