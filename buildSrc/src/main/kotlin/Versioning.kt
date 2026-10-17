// Shared versioning helpers, available unqualified in every build script
// (root and subprojects) because they live in buildSrc's default package,
// mirroring the behavior of the former Groovy root-script methods.

/**
 * Computes the version name. Honors the `VERSION` environment variable
 * (as set by CI, e.g. a ref like "refs/tags/6.3.1"), falling back to
 * `git describe --tags`, with a `-DIRTY` suffix when there are uncommitted changes.
 */
fun getTag(): String {
    val envVersion = System.getenv("VERSION") ?: "null"
    return if (envVersion == "null") {
        val dirty = if (runCommand("git", "diff-index", "--name-only", "HEAD", "--").isNotEmpty()) "-DIRTY" else ""
        val describe = runCommand("git", "describe", "--tags")
        "$describe$dirty"
    } else {
        val tagVersionToken = envVersion.split("/")
        if (tagVersionToken.size > 2) tagVersionToken[2] else tagVersionToken[0]
    }
}

/**
 * Returns the total number of commits on HEAD, used as the `versionCode`.
 */
@Suppress("unused")
fun getGitCommitCount(): Int {
    return runCommand("git", "rev-list", "HEAD", "--count").toInt()
}

private fun runCommand(vararg command: String): String {
    val process = ProcessBuilder(*command).start()
    val output = process.inputStream.bufferedReader().readText()
    process.waitFor()
    return output.trim()
}
