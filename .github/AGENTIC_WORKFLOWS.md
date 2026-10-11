# Agentic issue workflow

This repository defines a reviewable, label-driven pipeline:

1. `issue-triage` runs for new, reopened, or edited issues and assigns priority, kind, scope, and
   exactly one status.
2. `issue-rca` runs when `Status: Triaged` is added. It creates a draft pull request containing a
   failing regression test, then moves the issue and pull request to `Status: RCA Complete`.
3. `implement-fix` runs when the RCA pull request receives `Status: RCA Complete`. It moves the pull
   request to `Status: Fix In Progress` and assigns the `coding-agent` custom agent.
4. `pull-request-review` classifies and reviews every new or updated pull request, including pull
   requests from forks, without executing their code.
5. `assign-human-reviewer` requests review from `@carsonradtke` after the agent review is complete
   and the pull request is no longer a draft.
6. A human reviews and merges the completed pull request.

## Repository setup

Create these labels before enabling the workflows:

Create the following label families before enabling the workflows:

- Priority: `Priority: P0`, `Priority: P1`, `Priority: P2`, and `Priority: P3`.
- Kind: `Kind: Correctness`, `Kind: Performance`, `Kind: Safety`, `Kind: Compatibility`,
  `Kind: Build`, and `Kind: Documentation`.
- Status: `Status: Open`, `Status: Needs Info`, `Status: Triaged`, `Status: RCA Complete`,
  `Status: Fix In Progress`, `Status: Under Review`, and `Status: Blocked`.
- Scope as applicable: `Area: *`, `Platform: *`, `Compiler: *`, and `Standard: *`.
- Pull request risk: `Risk: Low`, `Risk: Medium`, `Risk: High`, and `Risk: Critical`.
- Pull request size: `Size: XS`, `Size: S`, `Size: M`, `Size: L`, and `Size: XL`.
- Review: `Review: Agent Reviewed` and `Review: Concerns Found`.
- Control: `Human Decision Required` and `Agent: Paused`.

The workflows cannot create labels. Status labels are mutually exclusive; each agent removes the
previous status when advancing an issue or pull request.

The label definitions are versioned in `.github/labels.json`. The `sync-labels.yml` workflow creates
or updates them after the manifest changes on `main`, or when run manually. It does not delete
labels that are absent from the manifest. To preview the operations locally:

```powershell
.github/scripts/sync-labels.ps1 -Repository Microsoft/GSL -WhatIf
```

The pipeline requires a fine-grained personal access token stored as the Actions secret
`GH_AW_AGENT_TOKEN`. GitHub does not start downstream workflows for events created with the default
`GITHUB_TOKEN`, so the scoped token is used for transition labels, RCA pull request creation, and
coding-agent assignment. Grant it access only to this repository with:

- Metadata: read
- Actions: read and write
- Contents: read and write
- Issues: read and write
- Pull requests: read and write

Copilot cloud agent and Copilot CLI organization policies must be enabled. For organization-billed
agentic workflow inference, enable "Allow use of Copilot CLI billed to the organization."

## Compile and review

Install the official extension and compile the Markdown workflows:

```shell
gh extension install github/gh-aw
gh aw compile
gh aw validate
```

Commit each `.md` source file together with its generated `.lock.yml` file. Never edit a lock file
directly.

Before enabling the full chain, run each workflow manually against a disposable test issue. Keep
branch protection, required status checks, and human approval enabled for agent-created pull
requests.

The workflows explicitly disable per-run and daily AI credit guardrails.

Pull requests from forks are treated as untrusted. The review workflows use `pull_request_target`
so they can post reviews and request reviewers, but they remain on the trusted base-repository
checkout and read fork diffs through the GitHub API. They never execute or check out fork code.
Write operations occur only through restricted safe outputs after threat detection.

### Pull request workflow security review

The `pull_request_target` trigger is intentional and has been reviewed as a security-sensitive
change. Both workflows set `checkout: false`; the generated framework checkout is limited to
trusted configuration from the base repository. The agents have no shell or edit tools, and fork
content is accessed only through read-only GitHub API tools. `GH_AW_AGENT_TOKEN` is isolated from
the agent runtime and is available only to restricted safe-output jobs for labels and reviewer
assignment.
