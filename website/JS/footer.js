function loadFooter(basePath) {
    document.getElementById("shared-footer-container").innerHTML = `
    <section class="u-align-center u-clearfix u-image u-shading custom-shared-footer">
      <div class="u-clearfix u-sheet u-valign-middle u-sheet-1">
        <div class="u-clearfix u-expanded-width-lg u-expanded-width-md u-expanded-width-xl u-layout-wrap u-layout-wrap-1">
          <div class="u-layout">
            <div class="u-layout-row">
              <div class="u-container-style u-layout-cell u-size-30 u-layout-cell-1">
                <div class="u-container-layout u-valign-middle-lg u-valign-middle-md u-valign-middle-xl u-container-layout-1">
                  <h3 class="u-align-center-sm u-align-center-xs u-text u-text-default u-text-1">Last Post</h3>
                  <div class="u-clearfix u-custom-html u-custom-html-1">
                    <ul class="style1">
                      <li><a href="${basePath}Infrarosu.html">Infrared communication</a></li>
                      <li><a href="${basePath}Aer.html">Air Quality Monitoring System V1</a></li>
                      <li><a href="${basePath}Cantar.html">Digital Scale</a></li>
                    </ul>
                  </div>
                </div>
              </div>
              <div class="u-container-style u-layout-cell u-size-30 u-layout-cell-2">
                <div class="u-container-layout u-valign-middle-sm u-valign-middle-xs u-container-layout-2">
                  <p class="u-large-text u-text u-text-default u-text-variant u-text-2"> Connect with me</p>
                  <div class="u-social-icons u-spacing-20 u-social-icons-1">
                    <a class="u-social-url" target="_blank" href="https://www.facebook.com/mer00s/" title="Facebook">
                      <span class="u-icon u-icon-circle u-social-facebook u-social-icon u-icon-1">
                        <svg class="u-svg-link" preserveAspectRatio="xMidYMin slice" viewBox="0 0 112.196 112.196"><use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#svg-d6ff"></use></svg>
                        <svg class="u-svg-content" viewBox="0 0 112.196 112.196" id="svg-d6ff"><g><circle style="fill:currentColor;" cx="56.098" cy="56.098" r="56.098"></circle><path style="fill:#FFFFFF;" d="M70.201,58.294h-10.01v36.672H45.025V58.294h-7.213V45.406h7.213v-8.34   c0-5.964,2.833-15.303,15.301-15.303L71.56,21.81v12.51h-8.151c-1.337,0-3.217,0.668-3.217,3.513v7.585h11.334L70.201,58.294z"></path></g></svg>
                      </span>
                    </a>
                    <a class="u-social-url" target="_blank" href="https://www.linkedin.com/in/sebastian-mihai-173174113/" title="Linkedin">
                      <span class="u-icon u-icon-circle u-social-icon u-social-linkedin u-icon-2">
                        <svg class="u-svg-link" preserveAspectRatio="xMidYMin slice" viewBox="0 0 112.196 112.196"><use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#svg-7ddf"></use></svg>
                        <svg class="u-svg-content" viewBox="0 0 112.196 112.196" id="svg-7ddf"><g><circle style="fill:#007AB9;" cx="56.098" cy="56.097" r="56.098"></circle><g><path style="fill:#F1F2F2;" d="M89.616,60.611v23.128H76.207V62.161c0-5.418-1.936-9.118-6.791-9.118    c-3.705,0-5.906,2.491-6.878,4.903c-0.353,0.862-0.444,2.059-0.444,3.268v22.524H48.684c0,0,0.18-36.546,0-40.329h13.411v5.715    c-0.027,0.045-0.065,0.089-0.089,0.132h0.089v-0.132c1.782-2.742,4.96-6.662,12.085-6.662    C83.002,42.462,89.616,48.226,89.616,60.611L89.616,60.611z M34.656,23.969c-4.587,0-7.588,3.011-7.588,6.967    c0,3.872,2.914,6.97,7.412,6.97h0.087c4.677,0,7.585-3.098,7.585-6.97C42.063,26.98,39.244,23.969,34.656,23.969L34.656,23.969z     M27.865,83.739H41.27V43.409H27.865V83.739z"></path></g></g></svg>
                      </span>
                    </a>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </section>
    
    <footer class="u-align-center u-clearfix u-footer u-palette-1-base u-footer">
      <div class="u-clearfix u-sheet u-sheet-1">
        <p class="u-small-text u-text u-text-variant u-text-1"> © 2021. All rights reserved. | Design by Sebastian Mihai.</p>
      </div>
    </footer>
    `;
}