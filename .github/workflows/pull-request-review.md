---
description: "Classify and review every new or updated pull request."
on:
  pull_request_target:
    types: [opened, reopened, synchronize]
  roles: [read, triage, write, maintain, admin]
  status-comment: true

checkout: false

permissions:
  contents: read
  issues: read
  pull-requests: read
  copilot-requests: write

network: defaults

tools:
  github:
    toolsets: [repos, issues, pull_requests]

safe-outputs:
  create-pull-request-review-comment:
    target: triggering
    max: 10
  submit-pull-request-review:
    target: triggering
    allowed-events: [COMMENT]
    footer: "always"
  add-labels:
    allowed:
      - "Priority: *"
      - "Kind: *"
      - "Risk: *"
      - "Size: *"
      - "Area: *"
      - "Platform: *"
      - "Compiler: *"
      - "Standard: *"
      - "Review: Agent Reviewed"
      - "Review: Concerns Found"
      - "Human Decision Required"
    max: 12
    github-token: ${{ secrets.GH_AW_AGENT_TOKEN }}
  remove-labels:
    allowed:
      - "Priority: *"
      - "Kind: *"
      - "Risk: *"
      - "Size: *"
      - "Review: Concerns Found"
    blocked:
      - "Agent: Paused"
    max: 8
    github-token: ${{ secrets.GH_AW_AGENT_TOKEN }}

timeout-minutes: 30
max-ai-credits: -1
max-daily-ai-credits: -1
---

# Classify and review the triggering pull request

Follow `.github/agents/pr-review-agent.agent.md`.

Review the current diff through the GitHub API against the pull request's base branch. The workflow
runs in the trusted base-repository context. Do not check out, execute, source, or import code,
scripts, build steps, or tests from the pull request because fork content is untrusted.

Replace obsolete priority, kind, risk, size, and review-result labels with the current
classification. Submit inline comments for concrete findings and a consolidated `COMMENT` review
that summarizes:

- What the pull request changes.
- Correctness, safety, compatibility, performance, and portability risks.
- Test coverage and validation gaps.
- Whether a human decision is required.

Add `Review: Agent Reviewed` only after submitting the review. Add `Review: Concerns Found` when the
human reviewer should focus on substantial or blocking concerns. Never approve or merge.
