#!/bin/bash
# =============================================================================
# RDA Message Board - Release Automation Script
# =============================================================================
# Features:
#   - Reads version from platformio.ini (single source of truth)
#   - Generates a categorized CHANGELOG.md from git commits
#   - Creates an annotated tag with rich commit summary
#   - Pushes to both Forgejo (origin) and GitHub (github) remotes
#   - Creates a GitHub release via API (requires GITHUB_TOKEN env var)
#   - Supports --dry-run, --force, --no-changelog flags
#
# Commit categories (prefix your commits with these for best results):
#   feat:, fix:, improve:, docs:, chore:, build:, ci:, refactor:, style:
#   Breaking changes: breaking: or BREAKING CHANGE in message
#
# Usage:
#   ./release.sh [commit_message] [--force] [--dry-run] [--no-changelog]
#
# GitHub Release creation (optional):
#   Export GITHUB_TOKEN before running:
#   export GITHUB_TOKEN="your_personal_access_token"
# =============================================================================

set -e

# -----------------------------------------------------------------------------
# Colors
# -----------------------------------------------------------------------------
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

print_status()  { echo -e "${BLUE}[INFO]${NC} $1"; }
print_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
print_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
print_error()   { echo -e "${RED}[ERROR]${NC} $1"; }
print_header()  { echo -e "${CYAN}$1${NC}"; }

# -----------------------------------------------------------------------------
# Configuration
# -----------------------------------------------------------------------------
REMOTES=("origin" "github")          # Remotes to push to
GITHUB_REPO="rdeangel/rda_msg_board" # GitHub user/repo for links and releases
CHANGELOG_FILE="CHANGELOG.md"

# -----------------------------------------------------------------------------
# Argument parsing
# -----------------------------------------------------------------------------
FORCE=false
DRY_RUN=false
NO_CHANGELOG=false
COMMIT_MESSAGE="Update code"

while [[ $# -gt 0 ]]; do
    case $1 in
        --force|-f)
            FORCE=true
            shift ;;
        --dry-run|-n)
            DRY_RUN=true
            shift ;;
        --no-changelog)
            NO_CHANGELOG=true
            shift ;;
        --help|-h)
            echo "Usage: $0 [commit_message] [options]"
            echo ""
            echo "Options:"
            echo "  --force, -f         Recreate existing tag and trigger a new release"
            echo "  --dry-run, -n       Show what would happen without making changes"
            echo "  --no-changelog      Skip CHANGELOG.md generation"
            echo "  --help, -h          Show this help"
            echo ""
            echo "Environment variables:"
            echo "  GITHUB_TOKEN        GitHub personal access token for release creation"
            echo ""
            echo "Examples:"
            echo "  ./release.sh 'Add sleep mode feature'"
            echo "  ./release.sh --dry-run"
            echo "  ./release.sh 'Hotfix' --force"
            exit 0 ;;
        *)
            COMMIT_MESSAGE="$1"
            shift ;;
    esac
done

# -----------------------------------------------------------------------------
# Helpers
# -----------------------------------------------------------------------------

# Push main branch + optional tag to all configured remotes
push_all() {
    local tag="$1"
    for remote in "${REMOTES[@]}"; do
        print_status "Pushing to ${remote}..."
        git push "$remote" main
        if [ -n "$tag" ]; then
            git push "$remote" "$tag"
        fi
        print_success "  ✅ ${remote} done"
    done
}

# Categorize commits and return a markdown block
# $1 = commit range, $2 = github_repo (for links, optional)
generate_release_notes() {
    local range="$1"
    local repo="$2"

    # Collect commits as "hash|subject"
    local all_commits
    all_commits=$(git log --format="%h|%s" "$range" 2>/dev/null || true)

    if [ -z "$all_commits" ]; then
        echo "_No commits found in range._"
        return
    fi

    # --- Categorize in priority order ---
    local breaking security features fixes improvements docs style tests chore build deps refactor removals remaining uncategorized

    breaking=$(    echo "$all_commits" | grep -iE "\|breaking:|\|BREAKING CHANGE|\|.*!:"                                           || true)
    security=$(    echo "$all_commits" | grep -iE "\|security:|\|sec:|\|vulnerability|\|CVE"                                       || true)
    remaining=$(   echo "$all_commits" | grep -ivE "\|breaking:|\|BREAKING CHANGE|\|.*!:|\|security:|\|sec:|\|vulnerability|\|CVE" || true)

    features=$(    echo "$remaining"   | grep -iE "\|feat:|\|add:"                     || true)
    fixes=$(       echo "$remaining"   | grep -iE "\|fix:|\|bug:"                      || true)
    remaining=$(   echo "$remaining"   | grep -ivE "\|feat:|\|add:|\|fix:|\|bug:"      || true)

    docs=$(        echo "$remaining"   | grep -iE "\|docs:|\|documentation:|\|readme:" || true)
    style=$(       echo "$remaining"   | grep -iE "\|style:|\|format:|\|lint:"         || true)
    tests=$(       echo "$remaining"   | grep -iE "\|test:|\|tests:|\|spec:"           || true)
    chore=$(       echo "$remaining"   | grep -iE "\|chore:|\|maintenance:"            || true)
    build=$(       echo "$remaining"   | grep -iE "\|build:|\|ci:|\|deploy:"           || true)
    deps=$(        echo "$remaining"   | grep -iE "\|deps:|\|dependencies:|\|package:" || true)
    refactor=$(    echo "$remaining"   | grep -iE "\|refactor:|\|restructure:"         || true)
    removals=$(    echo "$remaining"   | grep -iE "\|remove:|\|delete:|\|clean:"       || true)
    remaining=$(   echo "$remaining"   | grep -ivE "\|docs:|\|documentation:|\|readme:|\|style:|\|format:|\|lint:|\|test:|\|tests:|\|spec:|\|chore:|\|maintenance:|\|build:|\|ci:|\|deploy:|\|deps:|\|dependencies:|\|package:|\|refactor:|\|restructure:|\|remove:|\|delete:|\|clean:" || true)

    improvements=$(echo "$remaining"   | grep -iE "improve|enhance|update|refactor|optimize|debug" || true)
    uncategorized=$(echo "$remaining"  | grep -ivE "improve|enhance|update|refactor|optimize|debug" || true)
    # Strip meta commits
    uncategorized=$(echo "$uncategorized" | grep -ivE "\|Release v|\|chore: prepare release|\|chore: update CHANGELOG" || true)

    # Write each section to a temp file (avoids nested-function scope / subshell issues)
    local tmp
    tmp=$(mktemp)

    _append_section() {
        local icon="$1" title="$2" commits_data="$3" limit="${4:-10}"
        [ -z "$commits_data" ] && return
        printf "\n### %s %s\n" "$icon" "$title" >> "$tmp"
        echo "$commits_data" | head -"$limit" | while IFS='|' read -r hash msg; do
            if [ -n "$repo" ]; then
                printf -- "- %s - [%s](https://github.com/%s/commit/%s)\n" \
                    "$msg" "$hash" "$repo" "$hash" >> "$tmp"
            else
                printf -- "- %s (%s)\n" "$msg" "$hash" >> "$tmp"
            fi
        done
    }

    _append_section "⚠️"  "Breaking Changes"  "$breaking"      5
    _append_section "🔒"  "Security"          "$security"      5
    _append_section "✨"  "New Features"      "$features"      10
    _append_section "🐛"  "Bug Fixes"         "$fixes"         10
    _append_section "🚀"  "Improvements"      "$improvements"  10
    _append_section "📚"  "Documentation"     "$docs"          5
    _append_section "♻️"  "Refactor"          "$refactor"      5
    _append_section "🏗️"  "Build / CI"        "$build"         5
    _append_section "📦"  "Dependencies"      "$deps"          5
    _append_section "🎨"  "Style"             "$style"         5
    _append_section "🧪"  "Tests"             "$tests"         5
    _append_section "🗑️"  "Removals"          "$removals"      5
    _append_section "📝"  "Chore"             "$chore"         5
    _append_section "🔀"  "Other"             "$uncategorized" 10

    cat "$tmp"
    rm -f "$tmp"
}


# Update or create CHANGELOG.md
update_changelog() {
    local version="$1"
    local range="$2"
    local date_str
    date_str=$(date +%Y-%m-%d)

    print_status "Updating ${CHANGELOG_FILE}..."

    # Create empty changelog if missing
    if [ ! -f "$CHANGELOG_FILE" ]; then
        cat > "$CHANGELOG_FILE" << 'EOF'
# Changelog

All notable changes to this project will be documented in this file.

<!-- releases -->
EOF
        print_status "Created ${CHANGELOG_FILE}"
    fi

    # Skip if this version is already in the changelog (avoid duplicates on --force reruns)
    if grep -q "^## \[${version}\]" "$CHANGELOG_FILE" 2>/dev/null; then
        print_status "CHANGELOG.md already has entry for ${version} — skipping"
        return
    fi

    # Generate the new section
    local notes_tmp
    notes_tmp=$(mktemp)
    printf "## [%s] - %s\n" "$version" "$date_str" >> "$notes_tmp"
    generate_release_notes "$range" "$GITHUB_REPO" >> "$notes_tmp"
    printf "\n" >> "$notes_tmp"
    local new_section
    new_section=$(cat "$notes_tmp")
    rm -f "$notes_tmp"

    # Insert after the <!-- releases --> marker (or before first ## heading)
    local tmp
    tmp=$(mktemp)
    if grep -q "<!-- releases -->" "$CHANGELOG_FILE"; then
        awk -v section="$new_section" \
            '/<!-- releases -->/ { print; print ""; print section; next } { print }' \
            "$CHANGELOG_FILE" > "$tmp"
    else
        awk -v section="$new_section" \
            'inserted==0 && /^## / { print section; print ""; inserted=1 } { print }' \
            "$CHANGELOG_FILE" > "$tmp"
    fi
    mv "$tmp" "$CHANGELOG_FILE"

    print_success "Updated ${CHANGELOG_FILE} for ${version}"
}

# GitHub release creation is handled automatically by GitHub Actions
# when a tag is pushed. No token management needed locally.

# =============================================================================
# MAIN
# =============================================================================

print_header "============================================"
print_header " RDA Message Board Release Automation"
print_header "============================================"
[ "$DRY_RUN" = true ] && print_warning "DRY RUN MODE — no changes will be made"
echo ""

# --- Navigate to repo root ---
REPO_ROOT="$(git rev-parse --show-toplevel 2>/dev/null || echo "")"
[ -z "$REPO_ROOT" ] && { print_error "Not in a git repository"; exit 1; }
cd "$REPO_ROOT"

# --- Read version ---
VERSION=$(grep -E "^version\s*=" platformio.ini | sed 's/.*=\s*//' | tr -d ' \r')
if [ -z "$VERSION" ]; then
    print_error "Could not extract version from platformio.ini"
    exit 1
fi
print_status "Version from platformio.ini: ${VERSION}"

# --- Determine commit range ---
LAST_TAG=$(git describe --tags --abbrev=0 2>/dev/null | grep -v "^${VERSION}$" | head -1 || echo "")
if [ -n "$LAST_TAG" ]; then
    COMMIT_RANGE="${LAST_TAG}..HEAD"
    print_status "Commits since last tag: ${LAST_TAG}"
else
    TOTAL=$(git rev-list --count HEAD 2>/dev/null || echo "0")
    if [ "$TOTAL" -le 50 ]; then
        COMMIT_RANGE="HEAD"
    else
        COMMIT_RANGE="HEAD~50..HEAD"
    fi
    print_status "No previous tag found — using last ${TOTAL} commits"
fi

COMMIT_COUNT=$(git log --oneline $COMMIT_RANGE 2>/dev/null | wc -l | tr -d ' ')
print_status "Commits to include: ${COMMIT_COUNT}"
echo ""

# =============================================================================
# CASE A: Tag already exists
# =============================================================================
if git rev-parse "$VERSION" >/dev/null 2>&1; then

    if [ "$FORCE" = false ]; then
        print_warning "Tag ${VERSION} already exists — performing regular commit only"
        print_warning "Use --force to recreate the tag and trigger a new release"
        echo ""

        # Check that local branch is not behind any remote before committing
        if [ "$DRY_RUN" = false ]; then
            for remote in "${REMOTES[@]}"; do
                git fetch "$remote" main 2>/dev/null || true
                LOCAL=$(git rev-parse HEAD)
                REMOTE_REF=$(git rev-parse "$remote/main" 2>/dev/null || echo "")
                if [ -n "$REMOTE_REF" ] && [ "$LOCAL" != "$REMOTE_REF" ]; then
                    BEHIND=$(git rev-list --count HEAD.."$remote/main" 2>/dev/null || echo "0")
                    if [ "$BEHIND" -gt 0 ]; then
                        print_error "Local branch is ${BEHIND} commit(s) behind ${remote}/main"
                        print_error "Run: git pull --rebase ${remote} main  — then re-run release.sh"
                        exit 1
                    fi
                fi
            done
        fi

        git add .
        if git commit -m "$COMMIT_MESSAGE"; then
            print_success "Changes committed"
        else
            print_status "Nothing to commit — pushing existing commits"
        fi
        if [ "$DRY_RUN" = false ]; then
            push_all
        else
            print_status "[dry-run] Would push to: ${REMOTES[*]}"
        fi

    else
        print_warning "Force mode: recreating tag ${VERSION}"

        if [ "$DRY_RUN" = false ]; then
            git tag -d "$VERSION" 2>/dev/null || true
            for remote in "${REMOTES[@]}"; do
                git push "$remote" --delete "$VERSION" 2>/dev/null || true
            done

            git add .
            git commit -m "Release ${VERSION} - ${COMMIT_MESSAGE}" || \
                print_status "No new changes to commit — proceeding with tag"

            if [ "$NO_CHANGELOG" = false ]; then
                update_changelog "$VERSION" "$COMMIT_RANGE"
                git add "$CHANGELOG_FILE"
                if ! git diff --cached --quiet "$CHANGELOG_FILE"; then
                    git commit -m "chore: update CHANGELOG.md for ${VERSION}"
                fi
            fi

            # Rich annotated tag (write to file to preserve all newlines)
            tag_tmp=$(mktemp)
            printf 'Release %s\n\n' "${VERSION}" >> "$tag_tmp"
            generate_release_notes "$COMMIT_RANGE" "$GITHUB_REPO" >> "$tag_tmp"
            git tag -a "$VERSION" -F "$tag_tmp"
            rm -f "$tag_tmp"
            print_success "Recreated tag: ${VERSION}"

            push_all "$VERSION"
            print_status "GitHub Actions will create the release with build artifacts"
        else
            print_status "[dry-run] Would force-recreate tag ${VERSION} and push to: ${REMOTES[*]}"
        fi

        echo ""
        print_success "========================================"
        print_success " Release ${VERSION} force-recreated!"
        print_success "========================================"
    fi

# =============================================================================
# CASE B: New version — create release
# =============================================================================
else
    print_status "New version detected — creating release ${VERSION}"
    echo ""

    # Preview changelog
    print_header "--- Changelog Preview ---"
    generate_release_notes "$COMMIT_RANGE" "$GITHUB_REPO"
    echo ""
    print_header "-------------------------"
    echo ""

    if [ "$DRY_RUN" = false ]; then
        git add .
        if git commit -m "Release ${VERSION} - ${COMMIT_MESSAGE}"; then
            print_success "Changes committed"
        else
            print_status "No staged changes — continuing with tag creation"
        fi

        # Update CHANGELOG.md
        if [ "$NO_CHANGELOG" = false ]; then
            update_changelog "$VERSION" "$COMMIT_RANGE"
            git add "$CHANGELOG_FILE"
            if ! git diff --cached --quiet "$CHANGELOG_FILE"; then
                git commit -m "chore: update CHANGELOG.md for ${VERSION}"
                print_success "CHANGELOG.md committed"
            fi
        fi

        # Rich annotated tag (write to file to preserve all newlines)
        tag_tmp=$(mktemp)
        printf 'Release %s\n\n' "${VERSION}" >> "$tag_tmp"
        generate_release_notes "$COMMIT_RANGE" "$GITHUB_REPO" >> "$tag_tmp"
        git tag -a "$VERSION" -F "$tag_tmp"
        rm -f "$tag_tmp"
        print_success "Created tag: ${VERSION}"

        push_all "$VERSION"
        print_status "GitHub Actions will create the release with build artifacts"

    else
        print_status "[dry-run] Would commit, tag ${VERSION}, update CHANGELOG, and push to: ${REMOTES[*]}"
    fi

    echo ""
    print_success "========================================"
    print_success " Release ${VERSION} created and pushed!"
    print_success " GitHub Actions will build the binaries"
    print_success "========================================"
fi
