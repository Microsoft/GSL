---
description: "Root-cause a triaged bug and open a failing-test draft pull request."
on:
  issues:
    types: [labeled]
    lock-for-agent: true
  status-comment: true
if: "github.event.label.name == format('Status{0} Triaged', ':')"

permissions:
  contents: read
  issues: read
  pull-requests: read
  copilot-requests: write

network: defaults

tools:
  github:
    toolsets: [default]

safe-outputs:
  create-pull-request:
    title-prefix: "[RCA] "
    labels: ["Status: RCA Complete"]
    draft: true
    max: 1
    auto-close-issue: false
    fallback-as-issue: true
    allowed-branches: ["agent/rca-*"]
    protected-files: fallback-to-issue
    github-token: ${{ secrets.GH_AW_AGENT_TOKEN }}
  add-labels:
    allowed: ["Status: RCA Complete", "Status: Blocked", "Human Decision Required"]
    required-labels: ["Status: Triaged"]
    max: 2
    github-token: ${{ secrets.GH_AW_AGENT_TOKEN }}
  remove-labels:
    allowed: ["Status: Triaged"]
    required-labels: ["Status: Triaged"]
    max: 1
    github-token: ${{ secrets.GH_AW_AGENT_TOKEN }}
  add-comment:
    required-labels: ["Status: Triaged"]
    max: 1

timeout-minutes: 45
max-ai-credits: -1
max-daily-ai-credits: -1
---

# Root-cause the triggering issue

Follow `.github/agents/rca-agent.agent.md`.

First confirm that the issue describes a bug, has enough information to reproduce, and does not have
`Agent: Paused`. If it cannot proceed, explain the blocker, set `Status: Blocked`, add
`Human Decision Required` when appropriate, and do not create a pull request.

Otherwise:

1. Reproduce the behavior and isolate the responsible implementation path.
2. Add the smallest regression test that demonstrates the defect and fails for the expected reason.
3. Do not implement the production fix.
4. Run the focused test under C++14 and C++20 where applicable.
5. Create a draft pull request from `agent/rca-${{ github.event.issue.number }}-<short-description>`.
6. In the pull request body, link the issue without using a closing keyword and document the
   reproduction, failure, root cause, affected configurations, and likely fix constraints.
7. Replace `Status: Triaged` with `Status: RCA Complete` only after the draft pull request is
   created successfully.

The draft pull request is the handoff artifact for the implementation agent and human reviewers.
